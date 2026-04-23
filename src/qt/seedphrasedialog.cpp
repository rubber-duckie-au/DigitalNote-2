// Copyright (c) 2024-2025 DigitalNote XDN developers
// Distributed under the MIT software license.
// SPDX-License-Identifier: MIT

#include "seedphrasedialog.h"
#include "walletmodel.h"
#include "guiutil.h"
#include "bip39/bip39_wallet.h"
#include <openssl/crypto.h>

#include <QCloseEvent>
#include <QClipboard>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>
#include <QInputDialog>
#include <QCheckBox>
#include <QFrame>
#include <QFont>
#include <QScrollArea>
#include <QGridLayout>
#include <QSizePolicy>

// ── Constructor / Destructor ────────────────────────────────────────────────

SeedPhraseDialog::SeedPhraseDialog(WalletModel *model, QWidget *parent)
    : QDialog(parent)
    , m_model(model)
{
    setWindowTitle(tr("Wallet Seed Phrase (BIP39)"));
    setMinimumSize(680, 520);
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose, false);  // caller owns lifetime

    setupUi();

    connect(&m_countdownTimer, &QTimer::timeout,
            this,              &SeedPhraseDialog::onCountdownTick);
    connect(&m_clipboardTimer, &QTimer::timeout,
            this,              &SeedPhraseDialog::onClipboardClearTick);
    m_clipboardTimer.setSingleShot(true);
}

SeedPhraseDialog::~SeedPhraseDialog()
{
    clearMnemonic();
}

// ── UI setup (programmatic — no .ui file dependency) ────────────────────────

void SeedPhraseDialog::setupUi()
{
    auto *root = new QVBoxLayout(this);
    root->setSpacing(12);
    root->setContentsMargins(16, 16, 16, 16);

    // ── Warning banner ──────────────────────────────────────────────────────
    auto *warnFrame = new QFrame;
    warnFrame->setFrameShape(QFrame::StyledPanel);
    warnFrame->setStyleSheet(
        "QFrame { background:#fff3cd; border:1px solid #ffc107; border-radius:4px; }"
        "QLabel { background:transparent; color:#856404; font-size:9pt; }");
    auto *warnLayout = new QHBoxLayout(warnFrame);
    auto *warnIcon  = new QLabel("⚠");
    warnIcon->setStyleSheet("font-size:20pt; color:#856404;");
    auto *warnText  = new QLabel(
        tr("<b>Keep your seed phrase private.</b><br>"
           "Anyone with these words can access all your funds. "
           "Write them down on paper and store them securely offline. "
           "Never enter them on a website or share them digitally."));
    warnText->setWordWrap(true);
    warnLayout->addWidget(warnIcon);
    warnLayout->addWidget(warnText, 1);
    root->addWidget(warnFrame);

    // ── Word-count selector ─────────────────────────────────────────────────
    auto *optRow = new QHBoxLayout;
    optRow->addWidget(new QLabel(tr("Mnemonic length:")));
    auto *wordCountCombo = new QComboBox;
    wordCountCombo->addItem(tr("12 words (128-bit)"),  static_cast<int>(BIP39Wallet::WordCount::Words12));
    wordCountCombo->addItem(tr("15 words (160-bit)"),  static_cast<int>(BIP39Wallet::WordCount::Words15));
    wordCountCombo->addItem(tr("18 words (192-bit)"),  static_cast<int>(BIP39Wallet::WordCount::Words18));
    wordCountCombo->addItem(tr("21 words (224-bit)"),  static_cast<int>(BIP39Wallet::WordCount::Words21));
    wordCountCombo->addItem(tr("24 words (256-bit) — Recommended"),
                                                       static_cast<int>(BIP39Wallet::WordCount::Words24));
    wordCountCombo->setCurrentIndex(4);  // default: 24
    wordCountCombo->setObjectName("wordCountCombo");
    connect(wordCountCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,           &SeedPhraseDialog::onWordCountChanged);
    optRow->addWidget(wordCountCombo);
    optRow->addStretch();
    root->addLayout(optRow);

    // ── Seed phrase display area ─────────────────────────────────────────────
    auto *seedGroup = new QGroupBox(tr("Your Seed Phrase"));
    auto *seedLayout = new QVBoxLayout(seedGroup);

    // Grid to display each word individually — cleaner than a text blob
    auto *wordGrid = new QWidget;
    wordGrid->setObjectName("wordGrid");
    wordGrid->setLayout(new QGridLayout);
    wordGrid->hide();
    seedLayout->addWidget(wordGrid);

    // Placeholder shown before reveal
    auto *placeholderLabel = new QLabel(tr("Your recovery phrase is a 24-word backup of your wallet password.\n\n"
           "If you forget your password, enter these words in Settings \u2192 Unlock Wallet \u2192 Forgot password?\n\n"
           "Note: This backs up your password only, not your keys. Keep your wallet.dat file backed up separately."));
    placeholderLabel->setObjectName("placeholderLabel");
    placeholderLabel->setAlignment(Qt::AlignCenter);
    placeholderLabel->setStyleSheet("color:#888; font-size:10pt;");
    seedLayout->addWidget(placeholderLabel);

    root->addWidget(seedGroup, 1);

    // ── Countdown / reveal row ───────────────────────────────────────────────
    auto *btnRow = new QHBoxLayout;

    auto *countdownLabel = new QLabel;
    countdownLabel->setObjectName("countdownLabel");
    countdownLabel->setStyleSheet("color:#555; font-size:9pt;");
    countdownLabel->hide();
    btnRow->addWidget(countdownLabel);

    btnRow->addStretch();

    auto *copyBtn = new QPushButton(tr("Copy to Clipboard"));
    copyBtn->setObjectName("copyBtn");
    copyBtn->setEnabled(false);
    connect(copyBtn, &QPushButton::clicked, this, &SeedPhraseDialog::onCopyClicked);
    btnRow->addWidget(copyBtn);

    auto *verifyBtn = new QPushButton(tr("Verify Phrase…"));
    verifyBtn->setObjectName("verifyBtn");
    verifyBtn->setEnabled(false);
    verifyBtn->setToolTip(tr("Enter your seed phrase back to confirm you recorded it correctly"));
    connect(verifyBtn, &QPushButton::clicked, this, &SeedPhraseDialog::onVerifyClicked);
    btnRow->addWidget(verifyBtn);

    auto *revealBtn = new QPushButton(tr("Reveal Seed Phrase"));
    revealBtn->setObjectName("revealBtn");
    revealBtn->setStyleSheet(
        "QPushButton { background:#d9534f; color:white; font-weight:bold; "
        "padding:6px 14px; border-radius:4px; border:none; }"
        "QPushButton:hover { background:#c9302c; }"
        "QPushButton:disabled { background:#aaa; }");
    connect(revealBtn, &QPushButton::clicked, this, &SeedPhraseDialog::onRevealClicked);
    btnRow->addWidget(revealBtn);

    root->addLayout(btnRow);

    // ── Close ────────────────────────────────────────────────────────────────
    auto *closeBtn = new QPushButton(tr("Close"));
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
    auto *closeRow = new QHBoxLayout;
    closeRow->addStretch();
    closeRow->addWidget(closeBtn);
    root->addLayout(closeRow);
}

// ── Slot implementations ─────────────────────────────────────────────────────

void SeedPhraseDialog::onWordCountChanged(int index)
{
    auto *combo = findChild<QComboBox*>("wordCountCombo");
    if (!combo) return;
    m_wordCount = static_cast<BIP39Wallet::WordCount>(combo->itemData(index).toInt());
    // Clear existing display when the user changes word count
    clearMnemonic();
}

bool SeedPhraseDialog::ensureUnlocked()
{
    if (!m_model) return false;
    if (m_model->getEncryptionStatus() == WalletModel::Locked) {
        WalletModel::UnlockContext ctx(m_model->requestUnlock());
        return ctx.isValid();
    }
    return true;
}

void SeedPhraseDialog::onRevealClicked()
{
    if (!ensureUnlocked()) {
        QMessageBox::warning(this, tr("Wallet Locked"),
            tr("Please unlock your wallet to reveal the seed phrase."));
        return;
    }

    // Disable the button and start the mandatory countdown
    auto *revealBtn = findChild<QPushButton*>("revealBtn");
    if (revealBtn) revealBtn->setEnabled(false);

    startCountdown(10);
}

void SeedPhraseDialog::startCountdown(int seconds)
{
    m_countdownSecondsLeft = seconds;

    auto *label = findChild<QLabel*>("countdownLabel");
    if (label) {
        label->setText(tr("Revealing in %1 seconds…").arg(m_countdownSecondsLeft));
        label->show();
    }

    m_countdownTimer.start(1000);
}

void SeedPhraseDialog::onCountdownTick()
{
    --m_countdownSecondsLeft;
    auto *label = findChild<QLabel*>("countdownLabel");

    if (m_countdownSecondsLeft > 0) {
        if (label)
            label->setText(tr("Revealing in %1 seconds…").arg(m_countdownSecondsLeft));
        return;
    }

    m_countdownTimer.stop();
    if (label) label->hide();

    // Derive the recovery mnemonic from the wallet passphrase.
    // We ask the user to enter their passphrase here so we can derive
    // the deterministic recovery phrase from it.
    // The wallet must be encrypted — unencrypted wallets have no passphrase
    // and therefore no recovery phrase.
    if (m_model->getEncryptionStatus() == WalletModel::Unencrypted) {
        QMessageBox::information(this, tr("Recovery Phrase Unavailable"),
            tr("Your wallet is not encrypted.<br><br>"
               "A recovery phrase is only available for encrypted wallets.<br><br>"
               "Go to <b>Settings \u2192 Encrypt Wallet</b> to encrypt your wallet "
               "and receive a 24-word recovery phrase."));
        auto *revealBtn = findChild<QPushButton*>("revealBtn");
        if (revealBtn) revealBtn->setEnabled(true);
        return;
    }

    // Ask for passphrase to derive the recovery mnemonic
    bool ok2 = false;
    QString passQStr = QInputDialog::getText(
        this,
        tr("Enter your wallet password"),
        tr("Enter your wallet password to display your recovery phrase:"),
        QLineEdit::Password,
        QString(),
        &ok2);

    if (!ok2 || passQStr.isEmpty()) {
        auto *revealBtn = findChild<QPushButton*>("revealBtn");
        if (revealBtn) revealBtn->setEnabled(true);
        return;
    }

    SecureString passphrase;
    std::string passStr = passQStr.toStdString();
    passphrase.assign(passStr.c_str(), passStr.size());
    OPENSSL_cleanse(const_cast<char*>(passStr.data()), passStr.size());

    SecureString mnemonic;
    bool ok = m_model->generateRecoveryMnemonic(passphrase, mnemonic);
    OPENSSL_cleanse(const_cast<char*>(passphrase.data()), passphrase.size());

    if (!ok) {
        QMessageBox::critical(this, tr("Recovery Phrase Error"),
            tr("Could not generate the recovery phrase.<br><br>"
               "This may mean your wallet was encrypted with an older version of "
               "DigitalNote. To enable recovery phrase support, go to "
               "<b>Settings \u2192 Decrypt Wallet</b>, then "
               "<b>Settings \u2192 Encrypt Wallet</b> to re-encrypt."));
        auto *revealBtn = findChild<QPushButton*>("revealBtn");
        if (revealBtn) revealBtn->setEnabled(true);
        return;
    }

    m_currentMnemonic = QString::fromStdString(
        std::string(mnemonic.begin(), mnemonic.end()));
    OPENSSL_cleanse(const_cast<char*>(mnemonic.data()), mnemonic.size());

    showMnemonic(m_currentMnemonic);
}

void SeedPhraseDialog::showMnemonic(const QString& words)
{
    QStringList wordList = words.split(' ', Qt::SkipEmptyParts);

    // Rebuild the word grid
    auto *gridWidget = findChild<QWidget*>("wordGrid");
    auto *placeholder = findChild<QLabel*>("placeholderLabel");
    if (!gridWidget) return;

    // Clear old grid
    QLayout *old = gridWidget->layout();
    QLayoutItem *item;
    while (old && (item = old->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    delete old;

    auto *grid = new QGridLayout(gridWidget);
    grid->setHorizontalSpacing(10);
    grid->setVerticalSpacing(8);

    const int cols = (wordList.size() <= 12) ? 3 : 4;
    for (int i = 0; i < wordList.size(); ++i) {
        int row = i / cols;
        int col = i % cols;

        auto *cell = new QFrame;
        cell->setFrameShape(QFrame::StyledPanel);
        cell->setStyleSheet(
            "QFrame { background:#f0f4f8; border:1px solid #c5cdd6; border-radius:4px; }");
        auto *cellLayout = new QHBoxLayout(cell);
        cellLayout->setContentsMargins(6, 4, 6, 4);

        auto *numLabel = new QLabel(QString::number(i + 1));
        numLabel->setStyleSheet("color:#888; font-size:8pt; min-width:20px;");
        numLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        auto *wordLabel = new QLabel(wordList.at(i));
        wordLabel->setStyleSheet(
            "font-size:11pt; font-weight:bold; font-family:'Courier New',monospace; color:#1a1a1a;");
        wordLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

        cellLayout->addWidget(numLabel);
        cellLayout->addWidget(wordLabel, 1);
        grid->addWidget(cell, row, col);
    }

    gridWidget->show();
    if (placeholder) placeholder->hide();

    // Enable action buttons
    auto *copyBtn   = findChild<QPushButton*>("copyBtn");
    auto *verifyBtn = findChild<QPushButton*>("verifyBtn");
    auto *revealBtn = findChild<QPushButton*>("revealBtn");
    if (copyBtn)   copyBtn->setEnabled(true);
    if (verifyBtn) verifyBtn->setEnabled(true);
    if (revealBtn) revealBtn->setEnabled(false); // already revealed
}

void SeedPhraseDialog::onCopyClicked()
{
    if (m_currentMnemonic.isEmpty()) return;

    QClipboard *cb = QApplication::clipboard();
    cb->setText(m_currentMnemonic);

    auto *copyBtn = findChild<QPushButton*>("copyBtn");
    if (copyBtn) {
        copyBtn->setText(tr("Copied! (clears in 30s)"));
        copyBtn->setEnabled(false);
    }

    // Auto-clear clipboard after 30 seconds
    m_clipboardTimer.start(30000);
}

void SeedPhraseDialog::onClipboardClearTick()
{
    QClipboard *cb = QApplication::clipboard();
    // Only clear if our mnemonic is still on the clipboard
    if (cb->text() == m_currentMnemonic)
        cb->clear();

    auto *copyBtn = findChild<QPushButton*>("copyBtn");
    if (copyBtn) {
        copyBtn->setText(tr("Copy to Clipboard"));
        copyBtn->setEnabled(!m_currentMnemonic.isEmpty());
    }
}

void SeedPhraseDialog::onVerifyClicked()
{
    // Ask user to re-enter the mnemonic
    bool ok = false;
    QString entered = QInputDialog::getMultiLineText(
        this,
        tr("Verify Seed Phrase"),
        tr("Enter your seed phrase (space-separated words) to confirm you "
           "recorded it correctly:"),
        QString(), &ok);

    if (!ok || entered.trimmed().isEmpty()) return;

    entered = entered.simplified(); // normalise whitespace

    if (entered == m_currentMnemonic) {
        QMessageBox::information(this, tr("Verification Successful"),
            tr("✓ Your seed phrase matches. It is recorded correctly."));
    } else {
        SecureString ss(entered.toStdString().begin(), entered.toStdString().end());
        if (!BIP39Wallet::validateMnemonic(ss)) {
            QMessageBox::critical(this, tr("Invalid Mnemonic"),
                tr("The phrase you entered is not a valid BIP39 mnemonic "
                   "(checksum failed or unknown words)."));
        } else {
            QMessageBox::warning(this, tr("Mismatch"),
                tr("The phrase you entered does not match your wallet's recovery phrase. "
                   "Please check your written copy."));
        }
    }
}

// ── clearMnemonic ─────────────────────────────────────────────────────────────

void SeedPhraseDialog::clearMnemonic()
{
    if (!m_currentMnemonic.isEmpty()) {
        // Overwrite Qt's copy-on-write string in place
        for (int i = 0; i < m_currentMnemonic.size(); ++i)
            m_currentMnemonic[i] = QChar('\0');
        m_currentMnemonic.clear();
    }

    m_countdownTimer.stop();
    m_clipboardTimer.stop();

    // Reset grid
    auto *gridWidget  = findChild<QWidget*>("wordGrid");
    auto *placeholder = findChild<QLabel*>("placeholderLabel");
    auto *copyBtn     = findChild<QPushButton*>("copyBtn");
    auto *verifyBtn   = findChild<QPushButton*>("verifyBtn");
    auto *revealBtn   = findChild<QPushButton*>("revealBtn");

    if (gridWidget)  gridWidget->hide();
    if (placeholder) placeholder->show();
    if (copyBtn)   { copyBtn->setText(tr("Copy to Clipboard")); copyBtn->setEnabled(false); }
    if (verifyBtn)   verifyBtn->setEnabled(false);
    if (revealBtn)   revealBtn->setEnabled(true);
}

// ── Window events ─────────────────────────────────────────────────────────────

void SeedPhraseDialog::closeEvent(QCloseEvent *event)
{
    clearMnemonic();
    QDialog::closeEvent(event);
}

void SeedPhraseDialog::hideEvent(QHideEvent *event)
{
    clearMnemonic();
    QDialog::hideEvent(event);
}
