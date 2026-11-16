#ifndef CMASTERNODEMAN_H
#define CMASTERNODEMAN_H

#include <cstdint>
#include <vector>
#include <map>
#include <set>   // v2.0.0.9: setEverKnownPayees (blindness guard)

#include "ckeyid.h"   // v2.0.0.9 W-15 B: CKeyID in mAskedUsForMasternodeKey
#include "types/ccriticalsection.h"
#include "types/ctxdestination.h"
#include "cmnqueuesnapshot.h"

class CNode;
class CMasternode;
class CNetAddr;
class COutPoint;
class CService;
class CTxIn;
class CPubKey;
class CDataStream;
class CScript;
class CBlock;
class CBlockIndex;

class CMasternodeMan
{
private:
	// critical section to protect the inner data structures
	mutable CCriticalSection cs;

	// map to hold all MNs
	std::vector<CMasternode> vMasternodes;
	// who's asked for the masternode list and the last time
	std::map<CNetAddr, int64_t> mAskedUsForMasternodeList;
	// who we asked for the masternode list and the last time
	std::map<CNetAddr, int64_t> mWeAskedForMasternodeList;
	// which masternodes we've asked for
	std::map<COutPoint, int64_t> mWeAskedForMasternodeListEntry;

	// v2.0.0.9 FINDING-2026-002 (TODO 3.12b): dedup cache for inbound dseep
	// heartbeats.  Key = Hash(vin, sigTime, stop, vchSig); value = GetTime() of
	// first sight.  All access is inside the "dseep" handler, which runs under
	// cs_process_message (taken at the top of ProcessMessage), so these need no
	// separate lock.
	std::map<uint256, int64_t> mapSeenDseep;
	int64_t nLastDseepCleanup;

	// ----------------------------------------------------------------------
	// v2.0.0.8: chain-derived last-paid-height cache.
	//
	// mapLastPaidHeight[mn.vin.prevout] = the canonical chain height at which
	// this MN was most recently paid.  Populated by a bounded walk at startup
	// (PopulateLastPaidHeightCache) and maintained by OnBlockConnected /
	// OnBlockDisconnected hooks.
	//
	// NOT serialized to mncache.dat -- always rebuilt from chain on startup.
	// This guarantees v2.0.0.7 <-> v2.0.0.8 cache file compatibility and
	// avoids stale-data risk if cache and chain ever diverge.
	//
	// Consumers: GetQueuePaymentSnapshot (the vote-queue simulation in
	// CActiveMasternode::BroadcastQueue / SimulateQueue) and getmnlastpaid.
	// NOTE: CMasternode::nLastPaid is a SEPARATE field and is NOT display-only:
	// it is FindOldestNotInVec's ranking key (SecondsSincePayment), i.e. the
	// live mainnet payee pick before activation.  OnBlockConnected writes both.
	// ----------------------------------------------------------------------
	std::map<COutPoint, int> mapLastPaidHeight;

	// ----------------------------------------------------------------------
	// v2.0.0.8 PB-PAYEEDET Tier 2: chain-derived historical payee attestation.
	//
	// mapHistoricalPayees[payee_script] = the most recent chain height at which
	// this script appeared as an MN-payment-block payee.  Populated by:
	//   1. The PopulateLastPaidHeightCache walkback at startup (piggyback on
	//      the same chain walk -- no extra scan needed).
	//   2. The OnBlockAccepted hook in CBlock::SetBestChain (incremental
	//      update as new blocks become tip).
	//
	// PURPOSE.  The chain itself is the canonical record of which payees the
	// network consensus has previously vouched for.  Tier 1 of
	// IsPayeeAValidMasternode checks the LIVE vMasternodes view; Tier 2 falls
	// back to this CHAIN-DERIVED view for the cases where Tier 1 misses --
	// most notably during a resync that has caught up but whose dseg gossip
	// hasn't fully populated vMasternodes yet, or a historical block paying
	// an MN that has since decommissioned.
	//
	// SAFETY.  Tier 2 only relaxes the weak check; it does NOT replace the
	// stricter checks (CW12 voted-consensus, amount validation, devops
	// address match).  A script appearing in mapHistoricalPayees has chain-
	// committed precedent for being paid in this context -- a much higher
	// bar than "is currently in our local list".
	//
	// SIZE / PRUNING.  Capped at MAX_LASTPAID_SCAN_DEPTH (50000) DISTINCT
	// SCRIPTS; the lowest-height entries are evicted only past that cap.  With
	// ~30 payees the cap is never reached, so in practice there is NO age
	// limit: any script seen as a tip payee since startup (or in the startup
	// walk) stays attested for the life of the process.  Entries are not
	// removed on reorg, and blocks connected inside Reorganize are not
	// recorded (SetBestChain records only the final tip).  Ageing these out
	// is a v2.0.1.0 design item (V2).
	//
	// NOT serialized -- always rebuilt from chain on startup, same lifecycle
	// as mapLastPaidHeight.
	// ----------------------------------------------------------------------
	std::map<CScript, int> mapHistoricalPayees;

	// v2.0.0.9 blindness guard: every masternode payee script this node has EVER
	// held an entry for, this process.
	//
	// DELIBERATELY NOT SERIALISED.  A restarted node starts with this empty and
	// therefore considers itself blind until it has refreshed its list -- which
	// is exactly right, and it avoids changing the mncache.dat format.
	//
	// It is what lets IsRosterLikelyIncomplete() tell "a masternode I never knew
	// about" from "one I knew about that has since gone away" -- without it, every
	// departed masternode would look like evidence of blindness forever.
	std::set<CScript> setEverKnownPayees;

	// v2.0.0.9 W-15 option B: rate limit for the targeted "dsegk" self-lookup.
	//
	// Keyed by (peer IP, requested operator key).
	//
	// NOT BY IP ALONE -- that is the very bug W-15 is about: twenty co-hosted cold
	// masternodes share one address and each asks for a DIFFERENT key, so a per-IP
	// limit starves all but the first.
	//
	// NOT BY CService EITHER -- an inbound peer's port comes from accept() and is
	// EPHEMERAL (net.cpp:1090), so a per-port key never matches twice and the limit
	// would not exist at all.
	//
	// DELIBERATELY NOT SERIALISED: purely transient, so mncache.dat is unchanged.
	std::map<std::pair<CNetAddr, CKeyID>, int64_t> mAskedUsForMasternodeKey;

	// v2.0.0.8 PB-6: VESTIGIAL.  Formerly bounded RecomputeLastPaidHeight's
	// backward walk, but that bound was the PB-6 bug -- it is set to where
	// the startup scan terminated (a shallow recent height), which made
	// RecomputeLastPaidHeight miss payments deeper than that window.
	// RecomputeLastPaidHeight now walks by MAX_LASTPAID_SCAN_DEPTH instead.
	// This field is still written (constructor, PopulateLastPaidHeightCache)
	// but no longer read.  Left in place to avoid churn; do NOT reintroduce
	// it as a walk bound.
	int nLastPaidHeightScannedTo;

	// v2.0.0.9 rc9 (A6/C4): the walk behind RecomputeLastPaidHeight, on copies
	// of the MN's outpoint and payee script so no vMasternodes pointer is held
	// across the disk reads.  Takes cs only to write the result.
	void RecomputeLastPaidHeightFor(const COutPoint& outpoint, const CScript& mnScript,
	                                const CBlockIndex* pindexFrom);

public:
	// keep track of dsq count to prevent masternodes from gaming mnengine queue
	int64_t nDsqCount;

	CMasternodeMan();
	CMasternodeMan(CMasternodeMan& other);

	// Add an entry
	bool Add(CMasternode &mn);

	// Check all masternodes
	void Check();

	/// Ask (source) node for mnb
	void AskForMN(CNode* pnode, CTxIn &vin);

	// v2.0.0.8 PB-MN-FETCH Lite: fire-and-forget targeted request for the
	// peer's full mn list, triggered when CheckBlock encounters a payee not
	// in our local vMasternodes.  Rate-limited at 30s per (peer, payee) to
	// prevent amplification.  Returns true if a request was sent (caller
	// does not block waiting for the response).  See implementation.
	bool RequestMissingPayeeFromPeer(CNode* pnode, const CScript& payee);

	// v2.0.0.8 PB-PAYEEDET Tier 2: chain-derived historical attestation hook.
	// Called by CBlock::SetBestChain when a new block becomes the chain tip,
	// to record the block's MN payee in mapHistoricalPayees.  Idempotent
	// (later calls for the same payee update the height to the more recent
	// value).  Caller passes the resolved MN payee script -- this function
	// does not parse the block.
	void OnBlockAccepted(int nHeight, const CScript& mnPayee);

	// v2.0.0.8 PB-PAYEEDET Tier 2: query the chain-derived historical
	// attestation map.  Returns true if the payee has been recorded as an
	// MN-payment-slot payee since startup (no age limit; see the map's
	// SIZE / PRUNING note).  Used as a Tier-2 fallback from
	// IsPayeeAValidMasternode when the live vMasternodes check misses.
	bool IsPayeeHistoricallyAttested(const CScript& payee) const;

	// Check all masternodes and remove inactive
	void CheckAndRemove();

	// Clear masternode vector
	void Clear();

	int CountEnabled(int protocolVersion = -1);

	// v2.0.0.8 voted-consensus: deterministic, chain-derived eligible-voter
	// count for a given block height.  Consensus-denominator counterpart of
	// CountEnabled().  See implementation for the full rationale.
	// v2.0.0.9 (TODO 3.39): no protocol-version argument -- the denominator
	// does not filter on version any more.
	int CountVotingEligible(int nBlockHeight);

	int CountMasternodesAboveProtocol(int protocolVersion);

	void DsegUpdate(CNode* pnode);

	// Find an entry
	CMasternode* Find(const CTxIn& vin);
	CMasternode* Find(const CPubKey& pubKeyMasternode);

	//Find an entry thta do not match every entry provided vector
	CMasternode* FindOldestNotInVec(const std::vector<CTxIn> &vVins, int nMinimumAge);

	// Find a random entry
	CMasternode* FindRandom();

	/// Find a random entry
	CMasternode* FindRandomNotInVec(std::vector<CTxIn> &vecToExclude, int protocolVersion = -1);

	// Get the current winner for this block
	CMasternode* GetCurrentMasterNode(int mod=1, int64_t nBlockHeight=0, int minProtocol=0);

	// v2.0.0.8 PB-PAYEEDET: nBlockHeight is required to use the deterministic
	// IsVotingEligible(nBlockHeight) test (chain-derived collateral
	// confirmation depth + reorg buffer) instead of the non-deterministic
	// wall-clock IsEnabled() that was the root cause of post-restart
	// legacy-fallback chain splits.  Same architectural principle the CW12
	// strict path already uses; this applies it to the weak fallback path
	// that was missed in the original refactor.  All callers in this codebase
	// have the relevant height in scope (pindex->nHeight in validators,
	// pindexPrev->nHeight + 1 in builders).
	bool IsPayeeAValidMasternode(CScript payee, int nBlockHeight);
	std::vector<CMasternode> GetFullMasternodeVector();

	std::vector<std::pair<int, CMasternode>> GetMasternodeRanks(int64_t nBlockHeight, int minProtocol=0);
	int GetMasternodeRank(const CTxIn &vin, int64_t nBlockHeight, int minProtocol=0, bool fOnlyActive=true);
	CMasternode* GetMasternodeByRank(int nRank, int64_t nBlockHeight, int minProtocol=0, bool fOnlyActive=true);

	void ProcessMasternodeConnections();

	void ProcessMessage(CNode* pfrom, std::string& strCommand, CDataStream& vRecv);

	std::string ToString() const;
	int size();

	// ----------------------------------------------------------------------
	// v2.0.0.8: chain-derived last-paid-height cache (M1)
	//
	// All methods are thread-safe (acquire cs internally).
	// ----------------------------------------------------------------------


	// Called when a block is connected (SetBestChainInner, and per block
	// inside Reorganize).  Reads ONLY the block's masternode-slot output (the
	// CheckBlock index rule) and credits EVERY masternode whose payee script
	// matches it: mapLastPaidHeight = height and nLastPaid = block time.
	// v2.0.0.9 rc9 (review 3.1/3.2): it used to credit the first output paying
	// ANY masternode address -- so a stake returned to a collateral address was
	// recorded as a payment -- and only the first masternode with that address.
	void OnBlockConnected(const CBlock& block, int nBlockHeight);

	// Called from Reorganize for each disconnected block.  For every
	// masternode whose cached last-paid height is this block, recomputes it
	// by walking back from the disconnected block's PARENT (review 2.7: it
	// used to walk from the old tip and re-find the block being removed).
	// nLastPaid is not rewound on disconnect.
	void OnBlockDisconnected(const CBlock& block, int nBlockHeight);

	// Walk the chain backward from pindexFrom (default: the current tip),
	// looking for the most recent block whose masternode-slot output pays
	// this MN.  Walks up to MAX_LASTPAID_SCAN_DEPTH blocks and stops at the
	// first payment found; a never-paid MN costs the full 50,000 block reads,
	// and Add() calls this while holding cs (review 3.9 / V11).  If no payment
	// is found, removes the cache entry (treated as longest-ago-paid).
	void RecomputeLastPaidHeight(CMasternode* mn, const CBlockIndex* pindexFrom = NULL);

	// One-time at startup (after mncache.dat is loaded and the chain is
	// loaded): walk backward up to MAX_LASTPAID_SCAN_DEPTH blocks, recording
	// the most recent masternode-slot payment for each enabled MN (Tier 1)
	// and every slot payee in mapHistoricalPayees (Tier 2).  It does NOT stop
	// early: once every MN is found it keeps walking for Tier 2, so startup
	// always reads up to MAX_LASTPAID_SCAN_DEPTH blocks.
	void PopulateLastPaidHeightCache();

	// v2.0.0.9 blindness guard: does committed chain history show masternode
	// payees that we have never held an entry for?  See the implementation for
	// why this DELAYS the rescue rather than vetoing it.
	bool IsRosterLikelyIncomplete();

	// Runs the cs-taking scan and publishes the flag IsRosterLikelyIncomplete()
	// reads.  Call ONLY from a thread holding no locks (ThreadCheckMNenginePool)
	// -- see the LOCK ORDER note on the implementation.
	void RefreshRosterCompleteness();

	// Seed setEverKnownPayees from vMasternodes. REQUIRED after loading
	// mncache.dat: Unserialize() restores vMasternodes directly and bypasses
	// Add(), which is the only other place the set is populated.
	void RebuildEverKnownPayees();

	// Return cached lastPaidHeight for an MN.  Returns 0 if not found in the
	// cache (which means "never paid in our scanned range" -- treated as
	// longest-ago-paid by the vote-queue simulation).
	int GetLastPaidHeight(const COutPoint& vinPrevout) const;

	// v2.0.0.8 M1Q: snapshot every MN's chain-derived payment state under a
	// single cs acquisition (spec S18.1), for the queue forward-simulation
	// in CActiveMasternode::BroadcastQueue.  Returns one entry per MN in
	// vMasternodes.  Keeping cs private and exposing this purpose-built
	// accessor preserves the manager's lock encapsulation -- external code
	// never holds mnodeman.cs directly.
	std::vector<CMnPaymentSnapshotEntry> GetQueuePaymentSnapshot() const;

	// FINDING-2026-008: FindOldestNotInVecChainDerived() was DELETED 2026-08-07.
	// Vestigial scaffolding from the reverted PB-INFLIGHT / PB-16 experiment --
	// zero callers; all six live payee-selection sites use FindOldestNotInVec.
	// Chain-derived eligibility shipped via CMasternode::IsVotingEligible()
	// instead.  Rationale preserved in cmasternodevotetracker.cpp.

	//
	// Relay Masternode Messages
	//

	void RelayMasternodeEntry(const CTxIn vin, const CService addr, const std::vector<unsigned char> vchSig, const int64_t nNow, const CPubKey pubkey,
			const CPubKey pubkey2, const int count, const int current, const int64_t lastUpdated, const int protocolVersion, CScript donationAddress, int donationPercentage);
	void RelayMasternodeEntryPing(const CTxIn vin, const std::vector<unsigned char> vchSig, const int64_t nNow, const bool stop);

	// v2.0.0.9 FINDING-2026-013: relay a masternode's self-attested protocol
	// version.  Advisory telemetry only -- see CActiveMasternode::Mnver().
	void RelayMasternodeVersion(const CTxIn vin, const int nVersion, const int64_t nNow,
		const std::vector<unsigned char> vchSig);

	void Remove(CTxIn vin);

	unsigned int GetSerializeSize(int nType, int nVersion) const;
	template<typename Stream>
	void Serialize(Stream& s, int nType, int nVersion) const;
	template<typename Stream>
	void Unserialize(Stream& s, int nType, int nVersion);
};

#endif // CMASTERNODEMAN_H
