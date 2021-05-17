// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "wallet.h"
#include "main_const.h"

// Settings
int64_t nTransactionFee = MIN_TX_FEE;
int64_t nReserveBalance = 0;
int64_t nMinimumInputValue = 0;
int64_t nPoSageReward = 0;

// optional setting to unlock wallet for staking only
// serves to disable the trivial sendmoney when OS account compromised
// provides no real security
bool fWalletUnlockStakingOnly = false;
