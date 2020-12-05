#include "util.h"

#include "ckeypool.h"

CKeyPool::CKeyPool()
{
	nTime = GetTime();
}

CKeyPool::CKeyPool(const CPubKey& vchPubKeyIn)
{
	nTime = GetTime();
	vchPubKey = vchPubKeyIn;
}
