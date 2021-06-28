#include "compat.h"

#include "thread.h"
#include "cscript.h"
#include "util.h"
#include "ckey.h"
#include "cpubkey.h"
#include "script_const.h"
#include "ckeyid.h"
#include "cscriptid.h"

#include "cbasickeystore.h"

bool CBasicKeyStore::AddKeyPubKey(const CKey& key, const CPubKey &pubkey)
{
    LOCK(cs_KeyStore);
    
	mapKeys[pubkey.GetID()] = key;
	
    return true;
}

bool CBasicKeyStore::HaveKey(const CKeyID &address) const
{
	bool result;
	
	{
		LOCK(cs_KeyStore);
		
		result = (mapKeys.count(address) > 0);
	}
	
	return result;
}

void CBasicKeyStore::GetKeys(std::set<CKeyID> &setAddress) const
{
	setAddress.clear();
	
	{
		LOCK(cs_KeyStore);
		
		KeyMap::const_iterator mi = mapKeys.begin();
		
		while (mi != mapKeys.end())
		{
			setAddress.insert((*mi).first);
			mi++;
		}
	}
}

bool CBasicKeyStore::GetKey(const CKeyID &address, CKey &keyOut) const
{
	{
		LOCK(cs_KeyStore);
		
		KeyMap::const_iterator mi = mapKeys.find(address);
		
		if (mi != mapKeys.end())
		{
			keyOut = mi->second;
			
			return true;
		}
	}
	
	return false;
}

bool CBasicKeyStore::AddCScript(const CScript& redeemScript)
{
    if (redeemScript.size() > MAX_SCRIPT_ELEMENT_SIZE)
	{
        return error("CBasicKeyStore::AddCScript() : redeemScripts > %i bytes are invalid", MAX_SCRIPT_ELEMENT_SIZE);
	}
	
    LOCK(cs_KeyStore);
    
	mapScripts[redeemScript.GetID()] = redeemScript;
    
	return true;
}

bool CBasicKeyStore::HaveCScript(const CScriptID& hash) const
{
    LOCK(cs_KeyStore);
    
	return mapScripts.count(hash) > 0;
}

bool CBasicKeyStore::GetCScript(const CScriptID &hash, CScript& redeemScriptOut) const
{
    LOCK(cs_KeyStore);
    
	ScriptMap::const_iterator mi = mapScripts.find(hash);
    
	if (mi != mapScripts.end())
    {
        redeemScriptOut = (*mi).second;
        
		return true;
    }
    
	return false;
}

bool CBasicKeyStore::AddWatchOnly(const CScript &dest)
{
    LOCK(cs_KeyStore);
    
	setWatchOnly.insert(dest);
    
	return true;
}

bool CBasicKeyStore::RemoveWatchOnly(const CScript &dest)
{
    LOCK(cs_KeyStore);
    
	setWatchOnly.erase(dest);
    
	return true;
}

bool CBasicKeyStore::HaveWatchOnly(const CScript &dest) const
{
    LOCK(cs_KeyStore);
    
	return setWatchOnly.count(dest) > 0;
}

bool CBasicKeyStore::HaveWatchOnly() const
{
    LOCK(cs_KeyStore);
	
    return (!setWatchOnly.empty());
}

