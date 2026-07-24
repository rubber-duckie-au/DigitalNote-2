#include "rpcprotocol.h"
#include "net.h"
#include "net/cnodestats.h"
#include "cblock.h"
#include "mining.h"		// v2.0.0.9: RESCUE_STALL_SECS (consensus rescue)
#include "ctransaction.h"
#include "ctxout.h"
#include "ctxin.h"
#include "main_const.h"
#include "cpubkey.h"
#include "init.h"
#include "cwallet.h"
#include "cmasternodepayments.h"
#include "cmasternodevotetracker.h"
#include "util.h"
#include "chainparams.h"
#include "cchainparams.h"
#include "cbignum.h"
#include "main_extern.h"
#include "thread.h"			// v2.0.0.9 S3.15: LOCK macro (cs_main is extern in main_extern.h)
#include "cblockindex.h"
#include "miner.h"
#include "creservekey.h"

// v2.0.0.8.1: PoW consensus readiness gate implemented in rpcmining.cpp.
// Declared here as extern so mintblock can gate itself the same way.
// See rpcmining.cpp EnforceVotedConsensusReadyOrThrow for the full rationale.
static void EnforceVotedConsensusReadyOrThrow()
{
	if (pindexBest == NULL)
	{
		return;
	}

	int nNextHeight = pindexBest->nHeight + 1;
	int nActivationHeight = GetEffectiveVotedConsensusActivationHeight();

	if (nNextHeight < nActivationHeight)
	{
		return;
	}

	CScript votedPayeeProbe;
	bool fHaveCanonical = false;
	{
		LOCK(cs_main);

		fHaveCanonical = voteTracker.GetCanonicalWinnerFromQueues(
				nNextHeight, votedPayeeProbe);
	}

	if (!fHaveCanonical)
	{
		// v2.0.0.9 consensus rescue (v209-rescue-devops-fallback-SPEC): permit a
		// devops-fallback rescue block once the chain has been stalled
		// >= RESCUE_STALL_SECS (block-relative).  See rpcmining.cpp for rationale.
		if (ShouldMintRescueBlock(pindexBest, nNextHeight, GetAdjustedTime()))
		{
			LogPrintf("mintblock -- RESCUE: no voted winner for height %d and "
					  "chain stalled >= %ds; building a devops-fallback rescue "
					  "block.\n",
					  nNextHeight, (int)RESCUE_STALL_SECS);

			return;   // permit the build
		}

		static int64_t nLastGateLog = 0;
		int64_t nNow = GetTime();

		if (nNow - nLastGateLog >= 30)
		{
			nLastGateLog = nNow;
			LogPrintf("mintblock -- deferring: voted consensus active "
					  "for height %d (activation %d) but this node "
					  "has no canonical winner yet; refusing to build "
					  "a block the fleet would reject.\n",
					  nNextHeight, nActivationHeight);
		}

		throw JSONRPCError(-10,
				"voted consensus active but this node has no canonical "
				"winner for the next block; retry once vote tracker is "
				"ready");
	}
}

json_spirit::Value mintblock(const json_spirit::Array& params, bool fHelp)
{
	CBlock* block;
	CReserveKey* pMiningKey = NULL;

	pMiningKey = new CReserveKey(pwalletMain);

	// v2.0.0.8.1: PoW consensus readiness gate.  Refuses to build a
	// block if the node's own voted-consensus tracker cannot resolve
	// the next-height winner (matches getworkex/getwork/getblocktemplate
	// and the PoS ThreadStakeMiner gate).
	EnforceVotedConsensusReadyOrThrow();

	block = CreateNewBlock(*pMiningKey);

	bool fAccepted = ProcessBlock(NULL, block);
	if (!fAccepted)
	{
		return "rejected";
	}

	return block->ToString();
}




/*
// Example
json_spirit::Value mintblock(const json_spirit::Array& params, bool fHelp)
{
	CNodeStats stats;
	
	pnodeLocalHost->copyStats(stats);
	
	return stats.addrName;
}
*/