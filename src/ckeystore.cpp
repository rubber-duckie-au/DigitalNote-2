#include "ccriticalblock.h"
#include "pubkey.h"
#include "ckey.h"

#include "ckeystore.h"

CKeyStore::~CKeyStore()
{
	
}

bool CKeyStore::GetPubKey(const CKeyID &address, CPubKey &vchPubKeyOut) const
{
    CKey key;
    
	if (!GetKey(address, key))
	{
        return false;
    }
	
	vchPubKeyOut = key.GetPubKey();
	
    return true;
}

bool CKeyStore::AddKey(const CKey &key)
{
    return AddKeyPubKey(key, key.GetPubKey());
}