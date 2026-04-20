// Copyright (c) 2024 DigitalNote developers
// Distributed under the MIT software license.
// SPDX-License-Identifier: MIT

#include "coincontrolworker.h"
#include "sync.h"    // LOCK2, cs_main
#include "wallet.h"

CoinControlWorker::CoinControlWorker(CWallet *wallet,
                                     CCoinControl *coinControl,
                                     QObject *parent)
    : QObject(parent)
    , m_wallet(wallet)
    , m_coinControl(coinControl)
{
}

void CoinControlWorker::run()
{
    try {
        std::vector<COutput> vCoins;
        {
            // Hold both locks for the minimum time needed.
            LOCK2(cs_main, m_wallet->cs_wallet);
            m_wallet->AvailableCoins(vCoins, /*fOnlyConfirmed=*/true, m_coinControl);
        }

        QList<COutput> result;
        result.reserve(static_cast<int>(vCoins.size()));
        for (const COutput &out : vCoins)
            result.append(out);

        emit finished(result);
    } catch (const std::exception &e) {
        emit error(QString::fromStdString(e.what()));
    } catch (...) {
        emit error(QStringLiteral("Unknown error enumerating UTXOs"));
    }
}
