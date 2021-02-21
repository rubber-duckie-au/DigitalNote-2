// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_DB_H
#define BITCOIN_DB_H

#include <string>

class CDBEnv;

extern unsigned int nWalletDBUpdated;
extern CDBEnv bitdb;

void ThreadFlushWalletDB(const std::string& strWalletFile);

#endif // BITCOIN_DB_H
