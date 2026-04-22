// Copyright (c) 2024 DigitalNote developers
// Distributed under the MIT software license.
// SPDX-License-Identifier: MIT

#include "masternodeworker.h"

#include "cactivemasternode.h"
#include "mnengine_extern.h"
#include "init.h"
#include "cwallet.h"
#include "wallet.h"
#include "init.h"
#include "cwallet.h"

#include <boost/lexical_cast.hpp>

MasternodeWorker::MasternodeWorker(Operation op,
                                   std::vector<CMasternodeConfigEntry> entries,
                                   QObject *parent)
    : QObject(parent)
    , m_op(op)
    , m_entries(std::move(entries))
{
}

void MasternodeWorker::run()
{
    try {
        int total = static_cast<int>(m_entries.size());
        int successful = 0;
        int fail = 0;
        std::string statusObj;

        switch (m_op) {
        case StartSelected:
        case StartAll:
        {
            int idx = 0;
            for (const CMasternodeConfigEntry& mne : m_entries) {
                idx++;
                emit progress(idx, total, QString::fromStdString(mne.getAlias()));

                std::string errorMessage;
                std::string strDonateAddress;
                std::string strDonationPercentage;

                bool result = activeMasternode.Register(
                    mne.getIp(), mne.getPrivKey(),
                    mne.getTxHash(), mne.getOutputIndex(),
                    strDonateAddress, strDonationPercentage, errorMessage);

                if (result) {
                    successful++;
                } else {
                    fail++;
                    statusObj += "\nFailed to start " + mne.getAlias() + ". Error: " + errorMessage;
                }
            }

            if (pwalletMain)
                pwalletMain->Lock();

            std::string returnObj = "Successfully started " +
                boost::lexical_cast<std::string>(successful) +
                " masternode(s), failed to start " +
                boost::lexical_cast<std::string>(fail) +
                ", total " + boost::lexical_cast<std::string>(total);

            if (fail > 0)
                returnObj += statusObj;

            emit finished(QString::fromStdString(returnObj));
            break;
        }

        case StopSelected:
        case StopAll:
        {
            int idx = 0;
            for (const CMasternodeConfigEntry& mne : m_entries) {
                idx++;
                emit progress(idx, total, QString::fromStdString(mne.getAlias()));

                std::string errorMessage;
                bool result = activeMasternode.StopMasterNode(
                    mne.getIp(), mne.getPrivKey(), errorMessage);

                if (result) {
                    successful++;
                } else {
                    fail++;
                    statusObj += "\nFailed to stop " + mne.getAlias() + ". Error: " + errorMessage;
                }
            }

            if (pwalletMain)
                pwalletMain->Lock();

            std::string returnObj = "Successfully stopped " +
                boost::lexical_cast<std::string>(successful) +
                " masternode(s), failed " +
                boost::lexical_cast<std::string>(fail) +
                ", total " + boost::lexical_cast<std::string>(total);

            if (fail > 0)
                returnObj += statusObj;

            emit finished(QString::fromStdString(returnObj));
            break;
        }

        case Update:
            emit finished(QString());
            break;
        }

    } catch (const std::exception& e) {
        emit error(QString::fromStdString(e.what()));
    } catch (...) {
        emit error(QStringLiteral("Unknown error during masternode operation"));
    }
}
