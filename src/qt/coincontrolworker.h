// Copyright (c) 2024 DigitalNote developers
// Distributed under the MIT software license.
// SPDX-License-Identifier: MIT
//
// coincontrolworker.h — off-thread UTXO enumeration for CoinControlDialog

#pragma once

#include <QList>
#include <QObject>
#include <QString>

#include "wallet.h"       // COutput
#include "coincontrol.h"  // CCoinControl

class CWallet;

/**
 * @brief Enumerates available UTXOs off the GUI thread.
 *
 * Usage:
 * @code
 *   QThread *t       = new QThread(this);
 *   CoinControlWorker *w = new CoinControlWorker(wallet, coinCtrl);
 *   w->moveToThread(t);
 *   connect(t, &QThread::started,         w,    &CoinControlWorker::run);
 *   connect(w, &CoinControlWorker::finished, this, &MyDialog::onUtxosReady);
 *   connect(w, &CoinControlWorker::error,    this, &MyDialog::onWorkerError);
 *   connect(w, &CoinControlWorker::finished, t,    &QThread::quit);
 *   connect(w, &CoinControlWorker::error,    t,    &QThread::quit);
 *   connect(t, &QThread::finished,           t,    &QObject::deleteLater);
 *   connect(t, &QThread::finished,           w,    &QObject::deleteLater);
 *   t->start();
 * @endcode
 */
class CoinControlWorker : public QObject
{
    Q_OBJECT

public:
    explicit CoinControlWorker(CWallet *wallet,
                               CCoinControl *coinControl,
                               QObject *parent = nullptr);

public slots:
    void run();

signals:
    void finished(QList<COutput> utxos);
    void error(QString message);

private:
    CWallet      *m_wallet;
    CCoinControl *m_coinControl;
};
