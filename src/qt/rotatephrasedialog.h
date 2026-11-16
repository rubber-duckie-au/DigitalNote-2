// Copyright (c) 2024-2025 DigitalNote XDN developers
// Distributed under the MIT software license.
// SPDX-License-Identifier: MIT
//
// rotatephrasedialog.h
// Modal flow for rotating the wallet's recovery phrase ("compromised
// phrase" remediation).  Buried in advanced security settings -- NOT
// surfaced as a top-level menu item.
//
// User flow:
//   1. "Are you sure?" wall-of-text dialog explaining what rotation does
//      and does NOT protect against (in particular: copies of wallet.dat).
//   2. Re-enter current password (proves authorization; we use it to
//      re-encrypt the new vMasterKey under it).
//   3. Wallet is unlocked, master key is rotated, new mnemonic is computed.
//   4. New phrase is displayed in SeedPhraseDialog (FirstTimeAutoReveal
//      mode, the same dialog used after first encryption).  When it closes,
//      a confirmation box requires the tick-box "I have written down my new
//      24-word recovery phrase" before "Done" is enabled; "Show the phrase
//      again" (also Escape / close) re-opens the phrase.  The flow cannot
//      end until the user ticks and clicks Done (v2.0.0.9 rc9, ROT).
//   5. Old phrase no longer works for this wallet file (from the moment the
//      rotation commits, before step 4).
//
// v2.0.0.9 rc9 note (review 2 5.S2): the "done stage" members below
// (StageDone, buildDoneUi, the countdown / acknowledge / close slots and
// their widgets) are DEAD -- never reached since the flow moved to
// SeedPhraseDialog.  Removal is v2.0.1.0.
//
// All sensitive state is kept in SecureString and OPENSSL_cleansed before
// the dialog is destroyed.

#pragma once

#include <QDialog>
#include <QString>

class WalletModel;
class QLabel;
class QPushButton;
class QTextEdit;
class QCheckBox;
class QTimer;

class RotatePhraseDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RotatePhraseDialog(WalletModel *model, QWidget *parent = nullptr);
    ~RotatePhraseDialog() override;

protected:
    void closeEvent(QCloseEvent *event) override;
    void hideEvent (QHideEvent  *event) override;

private slots:
    void onCountdownTick();
    void onAcknowledgeToggled(bool checked);
    void onCloseClicked();

private:
    enum Stage {
        StageInit,      // not started yet
        StageDone,      // rotation complete, displaying new phrase
        StageFailed,    // something went wrong
    };

    // The full flow runs synchronously inside the constructor:
    //   1. show consent dialog,
    //   2. prompt for password,
    //   3. call WalletModel::rotateRecoveryPhrase,
    //   4. show the new phrase (SeedPhraseDialog) and loop until the user
    //      ticks "I have written down my new 24-word recovery phrase" and
    //      clicks Done (see the file header), then accept().
    void runFlow();

    bool showConsentDialog();
    bool promptForPassword(QString &outPassword);

    void buildDoneUi(const QString &newPhrase);
    void buildFailureUi(const QString &failureMessage);

    void clearAllSensitive();

    WalletModel  *m_model;
    Stage         m_stage;

    // Done-stage widgets
    QTextEdit    *m_phraseView;
    QLabel       *m_countdownLabel;
    QCheckBox    *m_ackCheck;
    QPushButton  *m_closeBtn;
    QTimer       *m_countdownTimer;
    int           m_secondsRemaining;
};
