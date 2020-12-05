// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "wallet.h"
#include "base58.h"
#include "blockparams.h"
#include "coincontrol.h"
#include "kernel.h"
#include "net.h"
#include "util.h"
#include "txdb.h"
#include "ui_interface.h"
#include "walletdb.h"
#include "crypter.h"
#include "key.h"
#include "spork.h"
#include "mnengine.h"
#include "instantx.h"
#include "masternodeman.h"
#include "masternode-payments.h"
#include "chainparams.h"
#include "smessage.h"
#include "webwalletconnector.h"
#include "fork.h"

#include <boost/algorithm/string/replace.hpp>

// Settings
int64_t nTransactionFee = MIN_TX_FEE;
int64_t nReserveBalance = 0;
int64_t nMinimumInputValue = 0;
int64_t nPoSageReward = 0;

// optional setting to unlock wallet for staking only
// serves to disable the trivial sendmoney when OS account compromised
// provides no real security
bool fWalletUnlockStakingOnly = false;



//////////////////////////////////////////////////////////////////////////////
//
// Actions
//

struct CompareByPriority
{
    bool operator()(const COutput& t1,
                    const COutput& t2) const
    {
        return t1.Priority() > t2.Priority();
    }
};


