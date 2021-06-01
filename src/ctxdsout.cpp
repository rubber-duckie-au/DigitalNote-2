#include "ctxdsout.h"

CTxDSOut::CTxDSOut(const CTxOut& out)
{
	nValue = out.nValue;
	nRounds = out.nRounds;
	scriptPubKey = out.scriptPubKey;
	nSentTimes = 0;
}

