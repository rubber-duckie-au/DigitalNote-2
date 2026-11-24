#ifndef CMERKLETX_H
#define CMERKLETX_H

#include <vector>

#include "uint/uint256.h"
#include "ctransaction.h"

class CBlockIndex;
class CBlock;

/** A transaction with a merkle branch linking it to the block chain. */
class CMerkleTx : public CTransaction
{
private:
	int GetDepthInMainChainINTERNAL(CBlockIndex* &pindexRet) const;

public:
	uint256 hashBlock;
	std::vector<uint256> vMerkleBranch;
	int nIndex;

	// memory only
	mutable bool fMerkleVerified;

	CMerkleTx();
	CMerkleTx(const CTransaction& txIn);

	unsigned int GetSerializeSize(int nType, int nVersion) const;
	template<typename Stream>
	void Serialize(Stream& s, int nType, int nVersion) const;
	template<typename Stream>
	void Unserialize(Stream& s, int nType, int nVersion);

	void Init();
	int SetMerkleBranch(const CBlock* pblock=NULL);

	// Return depth of transaction in blockchain:
	// -1  : not in blockchain, and not in memory pool (conflicted transaction)
	//  0  : in memory pool, waiting to be included in a block
	// >=1 : this many blocks deep in the main chain
	int GetDepthInMainChain(CBlockIndex* &pindexRet, bool enableIX=true) const;
	int GetDepthInMainChain(bool enableIX=true) const;
	bool IsInMainChain() const;
	int GetBlocksToMaturity() const;

	// v2.0.0.9 rc11: for callers that need several of the values above for
	// the same transaction (the GUI's balance totals), from ONE block look-up.
	// nChainDepth: > 0 exactly when IsInMainChain().  nDepthReal: what
	// GetDepthInMainChain(false) returns.  nDepthIX: what GetDepthInMainChain()
	// returns, computed only if fNeedIX (otherwise set to nDepthReal).
	void GetDepths(int& nChainDepth, int& nDepthReal, int& nDepthIX, bool fNeedIX) const;
	// GetBlocksToMaturity() for a transaction whose GetDepthInMainChain() is nDepthIX.
	int GetBlocksToMaturityAtDepth(int nDepthIX) const;
	bool AcceptToMemoryPool(bool fLimitFree=true, bool fRejectInsaneFee=true, bool ignoreFees=false);
	int GetTransactionLockSignatures() const;
	bool IsTransactionLockTimedOut() const;
};

#endif // CMERKLETX_H
