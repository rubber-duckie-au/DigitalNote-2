#ifndef CMASTERNODE_H
#define CMASTERNODE_H

#include <cstdint>
#include <vector>

#include "ctxin.h"
#include "net/cservice.h"
#include "cpubkey.h"
#include "types/ccriticalsection.h"

class uint256;
class CMasternode;
class CScript;

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

	// v2.0.0.9 FINDING-2026-013: version ATTESTED BY THE DAEMON ITSELF.
	//
	// protocolVersion above is stamped by whichever wallet last ran
	// 'masternode start' -- the collateral holder -- using THAT wallet's
	// compile-time PROTOCOL_VERSION.  It describes the CONTROLLER, not the
	// daemon running at this address, and on mainnet was observed wrong in both
	// directions.  Worse, it is a point-in-time stamp: upgrading a remote daemon
	// without re-running 'masternode start' leaves it stale indefinitely.
	//
	// These two fields carry the version the masternode reports about ITSELF,
	// signed with its own masternodeprivkey and refreshed on the ping cadence.
	//
	// DELIBERATELY NOT SERIALISED.  They are ephemeral -- refreshed at least
	// every MASTERNODE_PING_SECONDS -- so persisting them would only preserve
	// stale data across restarts, and adding fields to the CMasternode
	// serialisation would change the masternode.dat format for no benefit.
	//
	// DELIBERATELY NOT USED BY CONSENSUS.  CountVotingEligible() still reads
	// protocolVersion.  Wiring these in would be a consensus change requiring
	// its own activation and soak (FINDING-2026-014) -- keeping them advisory
	// is what makes this shippable without one.
	int nAttestedVersion;    // 0 = never attested
	int64_t nAttestedTime;   // sigTime of the attestation, for freshness/anti-replay

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
	CMasternode(CService newAddr, CTxIn newVin, CPubKey newPubkey, std::vector<unsigned char> newSig, int64_t newSigTime,
			CPubKey newPubkey2, int protocolVersionIn, CScript donationAddress, int donationPercentage);

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
	// v2.0.0.8 voted-consensus: deterministic, chain-derived voting eligibility.
	// Unlike IsEnabled() (which depends on wall-clock lastTimeSeen and is
	// therefore different on every node), IsVotingEligible(N) is a pure
	// function of committed chain state: it is true iff this MN's collateral
	// is confirmed at least VOTER_ELIGIBILITY_DEPTH blocks before height N.
	// This is the ONLY eligibility predicate that may feed a consensus rule.
	bool IsVotingEligible(int nBlockHeight) const;

	// v2.0.0.8 Spec B: the block height at which this MN's collateral tx
	// was confirmed on the active chain.  Pure committed-chain fact --
	// identical on every synced node -- so it is consensus-safe to use in
	// the candidate selector.  Returns -1 if the collateral tx cannot be
	// resolved on this node.
	int GetCollateralConfirmedHeight() const;

	int GetMasternodeInputAge();
	std::string Status();

	unsigned int GetSerializeSize(int nType, int nVersion) const;
	template<typename Stream>
	void Serialize(Stream& s, int nType, int nVersion) const;
	template<typename Stream>
	void Unserialize(Stream& s, int nType, int nVersion);
};

#endif // CMASTERNODE_H
