// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Darkcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef MASTERNODE_H
#define MASTERNODE_H

#include <vector>

#include "ctxin.h"
#include "net/cservice.h"
#include "cpubkey.h"
#include "cscript.h"
#include "ccriticalblock.h"

#include "masternode.h"

class uint256;

#define MASTERNODE_NOT_PROCESSED               0 // initial state
#define MASTERNODE_IS_CAPABLE                  1
#define MASTERNODE_NOT_CAPABLE                 2
#define MASTERNODE_STOPPED                     3
#define MASTERNODE_INPUT_TOO_NEW               4
#define MASTERNODE_PORT_NOT_OPEN               6
#define MASTERNODE_PORT_OPEN                   7
#define MASTERNODE_SYNC_IN_PROCESS             8
#define MASTERNODE_REMOTELY_ENABLED            9

#define MASTERNODE_MIN_CONFIRMATIONS           7
#define MASTERNODE_MIN_DSEEP_SECONDS           (30*60)
#define MASTERNODE_MIN_DSEE_SECONDS            (5*60)
#define MASTERNODE_PING_SECONDS                (1*60)
#define MASTERNODE_EXPIRATION_SECONDS          (65*60)
#define MASTERNODE_REMOVAL_SECONDS             (70*60)

class CMasternode;

bool GetBlockHash(uint256& hash, int nBlockHeight);

bool operator==(const CMasternode& a, const CMasternode& b);
bool operator!=(const CMasternode& a, const CMasternode& b);

//
// The Masternode Class. For managing the mnengine process. It contains the input of the 2,000,000 XDN, signature to prove
// it's the one who own that ip address and code for calculating the payment election.
//
class CMasternode
{
private:
    // critical section to protect the inner data structures
    mutable CCriticalSection cs;

public:
	enum state
	{
		MASTERNODE_ENABLED = 1,
		MASTERNODE_EXPIRED = 2,
		MASTERNODE_VIN_SPENT = 3,
		MASTERNODE_REMOVE = 4,
		MASTERNODE_POS_ERROR = 5
	};

	CTxIn vin;  
	CService addr;
	CPubKey pubkey;
	CPubKey pubkey2;
	std::vector<unsigned char> sig;
	int activeState;
	int64_t sigTime; //dsee message times
	int64_t lastDseep;
	int64_t lastTimeSeen;
	int cacheInputAge;
	int cacheInputAgeBlock;
	bool unitTest;
	bool allowFreeTx;
	int protocolVersion;
	int64_t nLastDsq; //the dsq count from the last dsq broadcast of this node
	CScript donationAddress;
	int donationPercentage;
	int nVote;
	int64_t lastVote;
	int nScanningErrorCount;
	int nLastScanningErrorBlockHeight;
	int64_t nLastPaid;
	bool isPortOpen;
	bool isOldNode;

	CMasternode();
	CMasternode(const CMasternode& other);
	CMasternode(CService newAddr, CTxIn newVin, CPubKey newPubkey, std::vector<unsigned char> newSig, int64_t newSigTime, CPubKey newPubkey2,
			int protocolVersionIn, CScript donationAddress, int donationPercentage);

	void swap(CMasternode& first, CMasternode& second);
	
	CMasternode& operator=(CMasternode from);
	friend bool operator==(const CMasternode& a, const CMasternode& b);
	friend bool operator!=(const CMasternode& a, const CMasternode& b);

	uint256 CalculateScore(int mod=1, int64_t nBlockHeight=0);

	int64_t SecondsSincePayment();
	void UpdateLastSeen(int64_t override=0);
	void ChangePortStatus(bool status);
	void ChangeNodeStatus(bool status);
	uint64_t SliceHash(uint256& hash, int slice);
	void Check();
    bool UpdatedWithin(int seconds);
    void Disable();
    bool IsEnabled();
    int GetMasternodeInputAge();
    std::string Status();
	
	unsigned int GetSerializeSize(int nType, int nVersion) const;
    template<typename Stream>
    void Serialize(Stream& s, int nType, int nVersion) const;
    template<typename Stream>
    void Unserialize(Stream& s, int nType, int nVersion);
};

#endif
