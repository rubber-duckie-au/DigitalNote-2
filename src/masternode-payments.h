// Copyright (c) 2014-2015 The Dash developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef MASTERNODE_PAYMENTS_H
#define MASTERNODE_PAYMENTS_H

#include <string>

class CNode;
class CDataStream;

void ProcessMessageMasternodePayments(CNode* pfrom, std::string& strCommand, CDataStream& vRecv);

#endif // MASTERNODE_PAYMENTS_H
