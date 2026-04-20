// Copyright (c) 2024 DigitalNote developers
// Distributed under the MIT software license.
// SPDX-License-Identifier: MIT
//
// sendcoinsworker.h — off-thread transaction building and broadcast

#pragma once

#include <QList>
#include <QObject>
#include <QString>

#include "walletmodel.h"  // WalletModel::SendCoinsReturn, SendCoinsRecipient

class CCoinControl;
class WalletModel;

/**
 * @brief Builds and broadcasts a transaction off the GUI thread.
 *
 * The GUI thread should:
 *  1. Disable the Send button.
 *  2. Start this worker.
 *  3. Re-enable the Send button in onSendFinished() / onSendError().
 */
class SendCoinsWorker : public QObject
{
    Q_OBJECT

public:
    explicit SendCoinsWorker(WalletModel *model,
                             QList<SendCoinsRecipient> recipients,
                             CCoinControl *coinControl,
                             QObject *parent = nullptr);

public slots:
    void run();

signals:
    /** result == WalletModel::OK on success; txid is the hex transaction id. */
    void finished(WalletModel::SendCoinsReturn result, QString txid);
    void error(QString message);

private:
    WalletModel               *m_model;
    QList<SendCoinsRecipient>  m_recipients;
    CCoinControl              *m_coinControl;
};
