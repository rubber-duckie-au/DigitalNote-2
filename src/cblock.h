#ifndef CBLOCK_H
#define CBLOCK_H

#include <cstdint>
#include <vector>
#include <memory>

#include "uint/uint256.h"

class CBlockIndex;
class COutPoint;
class CTxDB;
class CWallet;
class CBlock;
class CTransaction;
class CScript;
class CTxIn;
class CNode;  // v2.0.0.8 PB-MN-FETCH Lite: pfrom parameter on CheckBlock

// v2.0.0.8 M4: defined in cblock.cpp.  Post-activation with a voted winner in
// this node's tracker: returns true and that payee, which CheckBlock's strict
// check then ENFORCES.  Otherwise it falls through to the legacy
// masternodePayments.GetBlockPayee, whose list is permanently empty, so it
// returns false and the caller chooses (validators stay permissive; producers
// use FindOldestNotInVec before activation, defer / rescue after it).  M5
// routes block CREATION through this same hook.
// fQuiet suppresses the "no consensus; falling back to legacy" log line.
// Pass true from READ-ONLY query callers: `masternode winners` calls this in a
// loop over 30 heights, so one RPC wrote up to 30 consensus-failure lines into
// debug.log that had nothing to do with block production. Confirmed on testnet
// 2026-10-07: all 17 such lines landed in the single second the RPC was run.
bool GetEnforcedPayee(int nBlockHeight, CScript &payeeOut, CTxIn &vinOut, bool fQuiet = false);
// v2.0.0.9 rescue -- PRODUCER POLICY ONLY.  NOT a validity predicate: never call
// this from CheckBlock().  Renamed from IsRescueActive() on 2026-07-23 so that a
// validator call reads as obviously wrong.  See FINDING-2026-009 and
// v209-rescue-fix-SPEC-2026-07-23.md.
bool ShouldMintRescueBlock(const CBlockIndex* pindexPrev, int nBlockHeight, int64_t nBlockTime);

// The voted-consensus activation height in force.  Mainnet: the compiled
// floor, INT_MAX (NOT SCHEDULED) in v2.0.0.9, and SPORK_15 is ignored.
// Testnet: 2000, or a lower SPORK_15 value.  Every activation gate reads this.
int GetEffectiveVotedConsensusActivationHeight();

typedef std::unique_ptr<CBlock> CBlockPtr;

//
// Legacy code
//
//#ifdef __GNUC__
//	#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
//	
//	/* Test for GCC < 6.3.0 */
//	#if GCC_VERSION > 60300
//		
//	#else
//		typedef std::auto_ptr<CBlock> CBlockPtr;
//	#endif
//#else
//	typedef std::unique_ptr<CBlock> CBlockPtr;
//#endif

/** Nodes collect new transactions into a block, hash them into a hash tree,
 * and scan through nonce values to make the block's hash satisfy proof-of-work
 * requirements.  When they solve the proof-of-work, they broadcast the block
 * to everyone and the block is added to the block chain.  The first transaction
 * in the block is a special one that creates a new coin owned by the creator
 * of the block.
 *
 * Blocks are appended to blk0001.dat files on disk.  Their location on disk
 * is indexed by CBlockIndex objects in memory.
 */
class CBlock
{
public:
	// header
	static const int CURRENT_VERSION = 7;
	int nVersion;
	uint256 hashPrevBlock;
	uint256 hashMerkleRoot;
	unsigned int nTime;
	unsigned int nBits;
	unsigned int nNonce;

	// network and disk
	std::vector<CTransaction> vtx;

	// ppcoin: block signature - signed by one of the coin base txout[N]'s owner
	std::vector<unsigned char> vchBlockSig;

	// memory only
	mutable std::vector<uint256> vMerkleTree;

	// Denial-of-service detection:
	mutable int nDoS;
	bool DoS(int nDoSIn, bool fIn) const;

	CBlock();

	unsigned int GetSerializeSize(int nType, int nVersion) const;
	template<typename Stream>
	void Serialize(Stream& s, int nType, int nVersion) const;
	template<typename Stream>
	void Unserialize(Stream& s, int nType, int nVersion);

	void SetNull();
	bool IsNull() const;
	uint256 GetHash() const;
	uint256 GetPoWHash() const;
	int64_t GetBlockTime() const;
	void UpdateTime(const CBlockIndex* pindexPrev);

	// entropy bit for stake modifier if chosen by modifier
	unsigned int GetStakeEntropyBit() const;
	// ppcoin: two types of block: proof-of-work or proof-of-stake
	bool IsProofOfStake() const;
	bool IsProofOfWork() const;
	std::pair<COutPoint, unsigned int> GetProofOfStake() const;
	// ppcoin: get max transaction timestamp
	int64_t GetMaxTransactionTime() const;
	uint256 BuildMerkleTree() const;
	std::vector<uint256> GetMerkleBranch(int nIndex) const;
	static uint256 CheckMerkleBranch(uint256 hash, const std::vector<uint256>& vMerkleBranch, int nIndex);

	bool WriteToDisk(unsigned int& nFileRet, unsigned int& nBlockPosRet);
	bool ReadFromDisk(unsigned int nFile, unsigned int nBlockPos, bool fReadTransactions=true);
	std::string ToString() const;

	bool DisconnectBlock(CTxDB& txdb, CBlockIndex* pindex);
	bool ConnectBlock(CTxDB& txdb, CBlockIndex* pindex, bool fJustCheck=false);
	bool ReadFromDisk(const CBlockIndex* pindex, bool fReadTransactions=true);
	bool SetBestChain(CTxDB& txdb, CBlockIndex* pindexNew);
	bool AddToBlockIndex(unsigned int nFile, unsigned int nBlockPos, const uint256& hashProof);
	// v2.0.0.8 PB-MN-FETCH Lite: pfrom is the peer that delivered this block,
	// or NULL if the block came from a non-network source (local miner,
	// startup verify pass, ConnectBlock internal path).  When non-NULL,
	// CheckBlock may fire fire-and-forget dseg requests to that peer on
	// encountering masternode payees not in our list, to speed mn-list
	// propagation catch-up.  See CMasternodeMan::RequestMissingPayeeFromPeer.
	bool CheckBlock(bool fCheckPOW=true, bool fCheckMerkleRoot=true, bool fCheckSig=true, CNode* pfrom=NULL) const;
	bool AcceptBlock();
	bool SignBlock(CWallet& keystore, int64_t nFees);
	bool CheckBlockSignature() const;
	void RebuildAddressIndex(CTxDB& txdb);

private:
	bool SetBestChainInner(CTxDB& txdb, CBlockIndex *pindexNew);
};

#endif // CBLOCK_H
