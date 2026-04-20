// Copyright (c) 2024 DigitalNote developers
// Distributed under the MIT software license.
// SPDX-License-Identifier: MIT

#include "sendcoinsworker.h"
#include "walletmodel.h"
#include "coincontrol.h"

SendCoinsWorker::SendCoinsWorker(WalletModel *model,
                                 QList<SendCoinsRecipient> recipients,
                                 CCoinControl *coinControl,
                                 QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_recipients(std::move(recipients))
    , m_coinControl(coinControl)
{
}

void SendCoinsWorker::run()
{
    try {
        // requestUnlock() must NOT be called from a worker thread on some
        // platforms (it may spawn a QDialog).  The send button handler must
        // request unlock BEFORE starting this thread, and pass the already-
        // unlocked wallet context here.
        //
        // If the wallet is already unlocked (no passphrase), this is a no-op.
        WalletModel::UnlockContext ctx(m_model->requestUnlock());
        if (!ctx.isValid()) {
            emit error(tr("Wallet could not be unlocked."));
            return;
        }

        CWalletTx wtx;
        WalletModel::SendCoinsReturn ret =
            m_model->sendCoins(m_recipients, m_coinControl, wtx);

        QString txid;
        if (ret.status == WalletModel::OK)
            txid = QString::fromStdString(wtx.GetHash().GetHex());

        emit finished(ret, txid);

    } catch (const std::exception &e) {
        emit error(QString::fromStdString(e.what()));
    } catch (...) {
        emit error(QStringLiteral("Unknown error during send"));
    }
}
