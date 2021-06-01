#include "ctxdsin.h"

CTxDSIn::CTxDSIn(const CTxIn& in)
{
	prevout = in.prevout;
	scriptSig = in.scriptSig;
	prevPubKey = in.prevPubKey;
	nSequence = in.nSequence;
	nSentTimes = 0;
	fHasSig = false;
}

