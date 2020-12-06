#include "util.h"

#include "cwalletkey.h"

CWalletKey::CWalletKey(int64_t nExpires)
{
	nTimeCreated = (nExpires ? GetTime() : 0);
	nTimeExpires = nExpires;
}

