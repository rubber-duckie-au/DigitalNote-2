#include "util.h"
#include "ctxin.h"
#include "ctxout.h"
#include "ctxdsin.h"
#include "ctxdsout.h"
#include "mnengine.h"

#include "cmnengineentry.h"

CMNengineEntry::CMNengineEntry()
{
	isSet = false;
	collateral = CTransaction();
	amount = 0;
}

/// Add entries to use for MNengine
bool CMNengineEntry::Add(const std::vector<CTxIn> vinIn, int64_t amountIn, const CTransaction collateralIn, const std::vector<CTxOut> voutIn)
{
	if(isSet)
	{
		return false;
	}

	for(const CTxIn& in : vinIn)
	{
		sev.push_back(in);
	}
	
	for(const CTxOut& out : voutIn)
	{
		vout.push_back(out);
	}
	
	amount = amountIn;
	collateral = collateralIn;
	isSet = true;
	addedTime = GetTime();

	return true;
}

bool CMNengineEntry::AddSig(const CTxIn& vin)
{
	for(CTxDSIn& s : sev)
	{
		if(s.prevout == vin.prevout && s.nSequence == vin.nSequence)
		{
			if(s.fHasSig)
			{
				return false;
			}
			
			s.scriptSig = vin.scriptSig;
			s.prevPubKey = vin.prevPubKey;
			s.fHasSig = true;

			return true;
		}
	}

	return false;
}

bool CMNengineEntry::IsExpired()
{
	return (GetTime() - addedTime) > MNengine_QUEUE_TIMEOUT;// 120 seconds
}

