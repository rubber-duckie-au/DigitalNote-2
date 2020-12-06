#include "caccount.h"

CAccount::CAccount()
{
	SetNull();
}

void CAccount::SetNull()
{
	vchPubKey = CPubKey();
}

