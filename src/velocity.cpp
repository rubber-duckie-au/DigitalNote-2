#include "ctxin.h"
#include "ctxout.h"
#include "ctxindex.h"
#include "blockparams.h"
#include "wallet.h"
#include "cblock.h"
#include "cblockindex.h"
#include "util.h"
#include "ctransaction.h"

#include "velocity.h"

/* VelocityI(int nHeight) ? i : -1
   Returns i or -1 if not found */
int VelocityI(int nHeight)
{
	int i = 0;

	i --;

	for(int h : VELOCITY_HEIGHT)
	{
		if( nHeight >= h )
		{
			i++;
		}
	}

	return i;
}

/* Velocity(int nHeight) ? true : false
   Returns true if nHeight is higher or equal to VELOCITY_HEIGHT */
bool Velocity_check(int nHeight)
{
	LogPrintf("Checking for Velocity on block %u: ",nHeight);

	if(VelocityI(nHeight) >= 0)
	{
		LogPrintf("Velocity is currently Enabled\n");
		
		return true;
	}

	LogPrintf("Velocity is currently disabled\n");

	return false;
}

/* Velocity(CBlockIndex* prevBlock, CBlock* block) -> VelocityResult
   Called near the top of CBlock::AcceptBlock.
   HOTFIX (v2.0.0.8.1): returns a 3-state enum instead of bool so the
   caller can distinguish severe rejections (DoS-worthy) from benign
   ones (clock skew). */
VelocityResult Velocity(CBlockIndex* prevBlock, CBlock* block)
{
	const mapPrevTx_t mapInputs;

	// Define values
	int64_t TXvalue = 0;
	int64_t TXinput = 0;
	int64_t TXfee = 0;
	int64_t TXdevfee = 0;
	int64_t TXnetfee = 0;
	int64_t TXcount = 0;
	int64_t TXlogic = 0;
	int64_t TXrate = 0;
	int64_t CURvalstamp  = 0;
	int64_t OLDvalstamp  = 0;
	int64_t CURstamp = 0;
	int64_t OLDstamp = 0;
	int64_t TXstampC = 0;
	int64_t TXstampO = 0;
	int64_t devopsPayment = 0;
	int64_t SYScrntstamp = 0;
	int64_t SYSbaseStamp = 0;
	int nHeight = prevBlock->nHeight+1;
	int i = VelocityI(nHeight);
	int HaveCoins = false;
	bool fpayment = true;

	// Set stanard values
	TXrate = block->GetBlockTime() - prevBlock->GetBlockTime();
	TXstampC = block->nTime;
	TXstampO = prevBlock->nTime;
	CURstamp = block->GetBlockTime();
	OLDstamp = prevBlock->GetBlockTime();
	CURvalstamp = prevBlock->GetBlockTime() + VELOCITY_MIN_RATE[i];
	// v2.0.0.8 PB-1 (companion fix): genesis-boundary guard.
	//
	// prevBlock->pprev is NULL when prevBlock is the genesis block.
	// The original `prevBlock->pprev->GetBlockTime()` dereferenced NULL
	// in that case -- the same null-CBlockIndex / GetBlockTime() crash
	// class as blockparams.cpp VRX_ThreadCurve.  Velocity() is called
	// near the top of CBlock::AcceptBlock; a block whose parent is
	// genesis would trip this.  When there is no grandparent, fall back
	// to prevBlock's own time so OLDvalstamp stays well-defined (the
	// pprev-derived value is only a looser lower-bound check than the
	// prevBlock-derived CURvalstamp above).
	if (prevBlock->pprev != NULL)
	{
		OLDvalstamp = prevBlock->pprev->GetBlockTime() + VELOCITY_MIN_RATE[i];
	}
	else
	{
		OLDvalstamp = prevBlock->GetBlockTime() + VELOCITY_MIN_RATE[i];
	}
	SYScrntstamp = GetAdjustedTime() + VELOCITY_MIN_RATE[i];
	SYSbaseStamp = GetTime() + VELOCITY_MIN_RATE[i];

	// TODO: Rework and activate below section for future releases
	// Factor in TXs for Velocity constraints only if there are TXs to do so with
	if(VELOCITY_FACTOR[i] == true && TXvalue > 0)
	{
		// Set factor values
		for(const CTransaction& tx : block->vtx)
		{
			TXvalue = tx.GetValueOut();
			TXinput = tx.GetValueMapIn(mapInputs);
			TXfee = TXinput - TXvalue;
			TXcount = block->vtx.size();
			// TXlogic = GetPrevAccountBalance - TXinput;
			// TXrate = block->GetBlockTime() - prevBlock->GetBlockTime();
		}
		
		// Set Velocity logic value
		if(TXlogic > 0)
		{
			HaveCoins = true;
		}
		
		// Check for and enforce minimum TXs per block (Minimum TXs are disabled for DigitalNote)
		if(VELOCITY_MIN_TX[i] > 0 && TXcount < VELOCITY_MIN_TX[i])
		{
			LogPrintf("DENIED: Not enough TXs in block\n");
			
			return VelocityResult::RejectedSevere;
		}
		
		// Authenticate submitted block's TXs
		if(VELOCITY_MIN_VALUE[i] > 0 || VELOCITY_MIN_FEE[i] > 0)
		{
			// Make sure we accept only blocks that sent an amount
			// NOT being more than available coins to send
			if(VELOCITY_MIN_FEE[i] > 0 && TXinput > 0)
			{
				if(HaveCoins == false)
				{
					LogPrintf("DENIED: Balance has insuficient funds for attempted TX with Velocity\n");
					
					return VelocityResult::RejectedSevere;
				}
			}
			
			if(VELOCITY_MIN_VALUE[i] > 0 && TXvalue < VELOCITY_MIN_VALUE[i])
			{
				LogPrintf("DENIED: Invalid TX value found by Velocity\n");
				
				return VelocityResult::RejectedSevere;
			}
			
			if(VELOCITY_MIN_FEE[i] > 0 && TXinput > 0)
			{
				if(TXfee < VELOCITY_MIN_FEE[i])
				{
					LogPrintf("DENIED: Invalid network fee found by Velocity\n");
					
					return VelocityResult::RejectedSevere;
				}
			}
		}
	}

	// Verify minimum Velocity rate.
	// Too-rapid block spacing is a real attack indicator (a malicious
	// staker producing back-to-back blocks faster than min spacing) and
	// remains DoS-worthy.
	if( VELOCITY_RATE[i] > 0 && TXrate >= VELOCITY_MIN_RATE[i] )
	{
		LogPrintf("CHECK_PASSED: block spacing has met Velocity constraints\n");
	}
	else if( VELOCITY_RATE[i] > 0 && TXrate < VELOCITY_MIN_RATE[i] )
	{
		LogPrintf("DENIED: Minimum block spacing not met for Velocity (severe)\n");
		
		return VelocityResult::RejectedSevere;
	}
	
	// HOTFIX (v2.0.0.8.1) -- REMOVED dead code at this location.
	// 
	// The original file had a third `else if` here that purported to
	// validate timestamps "based on previous block history".  It was
	// unreachable: VELOCITY_RATE[i] > 0 is always true on mainnet, so
	// either the first or second branch above always fires.  The
	// `else if` after a `return false` branch was structurally dead.
	//
	// The chain-history timestamp invariants the dead code was meant
	// to enforce (block_ts >= prev_block_ts + min_spacing, etc.) are
	// already covered by AcceptBlock()'s GetPastTimeLimit() /
	// FutureDrift() checks in cblock.cpp.  No semantic loss from
	// removing the dead branch.

	// HOTFIX (v2.0.0.8.1) -- WALL-CLOCK TIMESTAMP CHECK.
	//
	// The previous implementation used BLOCK_SPACING_MIN (45 seconds)
	// as the tolerance for "is this block's timestamp in the future
	// relative to our system clock?" and then returned `false`, which
	// the caller mapped to DoS(100) -> ban.  That combination caused
	// the 2026-06-13 self-isolation incident where a freshly-started
	// node banned 86% of its mainnet peers during the volatile
	// nTimeOffset bootstrap window.
	//
	// Three changes:
	//   1) Tolerance widened from 45s -> 180s.  Stake-grinding is
	//      bounded by STAKE_TIMESTAMP_MASK=15 (16-sec granularity),
	//      giving ~11 attempts per slot at 180s versus ~3 at 45s --
	//      not a meaningful security degradation.
	//   2) Severity is BENIGN: clock skew is noise, not malice.  The
	//      block is still rejected; the peer is not banned.
	//   3) Bootstrap grace period: the check is skipped entirely
	//      when our own time view is unstable (insufficient samples)
	//      or our offset is itself large.  A node whose
	//      nTimeOffset has not yet converged is in no position to
	//      judge another node's "future" claim.
	//
	// The full post-mortem is in v209-TODO.md.
	{
		static const int64_t WALLCLOCK_FUTURE_TOLERANCE     = 180; // seconds
		static const int     MIN_TIME_SAMPLES_FOR_ENFORCE   = 11;
		static const int64_t MAX_SELF_OFFSET_FOR_ENFORCE    = 60;

		int  nSamples    = GetTimeOffsetSampleCount();
		int64_t nOffset  = GetTimeOffset();
		bool fStableTime = (nSamples >= MIN_TIME_SAMPLES_FOR_ENFORCE)
		                && (abs64(nOffset) < MAX_SELF_OFFSET_FOR_ENFORCE);

		if (fStableTime)
		{
			int64_t SYSbaseTol = GetTime()         + WALLCLOCK_FUTURE_TOLERANCE;
			int64_t SYScrntTol = GetAdjustedTime() + WALLCLOCK_FUTURE_TOLERANCE;

			if (CURstamp > SYSbaseTol || CURstamp > SYScrntTol
			    || TXstampC > SYSbaseTol || TXstampC > SYScrntTol)
			{
				LogPrintf("DENIED: Block timestamp is not logical "
				          "(block_ts=%d, local+%ds=%d, adjusted+%ds=%d) "
				          "[benign -- clock skew, peer not banned]\n",
				          (int)CURstamp,
				          (int)WALLCLOCK_FUTURE_TOLERANCE, (int)SYSbaseTol,
				          (int)WALLCLOCK_FUTURE_TOLERANCE, (int)SYScrntTol);
				
				return VelocityResult::RejectedBenign;
			}
		}
		else
		{
			LogPrintf("Velocity wall-clock check skipped: time view not stable "
			          "(samples=%d, our offset=%ds -- not authoritative)\n",
			          nSamples, (int)nOffset);
		}
	}

	// Verify coinbase transaction includes DevOps payment
	if (1 > 2)
	{
		if(block->IsProofOfStake())
		{
			fpayment = false;
			devopsPayment = GetDevOpsPayment(nHeight, nPoSageReward);
			
			// Set block TX values
			for(const CTransaction& tx : block->vtx)
			{
				TXcount = block->vtx.size();
				TXvalue = tx.GetValueOut();
				TXinput = tx.GetValueMapIn(mapInputs);
				TXfee = TXinput - TXvalue;
				
				if(TXfee > devopsPayment)
				{
					TXnetfee = TXfee - devopsPayment;
				}
				else
				{
					TXnetfee = devopsPayment - TXfee;
				}
				
				if(TXfee > TXnetfee)
				{
					TXdevfee = TXfee - TXnetfee;
				}
				else
				{
					TXdevfee = TXnetfee - TXfee;
				}
			}

			if(TXdevfee == devopsPayment)
			{
				LogPrintf("CHECK_PASSED: block contains valid devops payment\n");
				
				fpayment = true;
			}
		}

		if (!fpayment)
		{
			LogPrintf("DENIED: block does not contain valid devops payment\n");
			
			return VelocityResult::RejectedSevere;
		}
	}

	// Constrain Velocity
	if(VELOCITY_EXPLICIT[i])
	{
		if(VELOCITY_MIN_TX[i] > 0)
		{
			return VelocityResult::RejectedSevere;
		}
		
		if(VELOCITY_MIN_VALUE[i] > 0)
		{
			return VelocityResult::RejectedSevere;
		}
		
		if(VELOCITY_MIN_FEE[i] > 0)
		{
			return VelocityResult::RejectedSevere;
		}
	}

	// Velocity constraints met, return block acceptance
	LogPrintf("ACCEPTED: block has met all Velocity constraints\n");

	return VelocityResult::Accepted;
}

