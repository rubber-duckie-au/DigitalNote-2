#include "ckeystore.h"
#include "stealth.h"
#include "cstealthaddress.h"

#include "ckeystoreisminevisitor.h"

CKeyStoreIsMineVisitor::CKeyStoreIsMineVisitor(const CKeyStore *keystoreIn) : keystore(keystoreIn)
{
	
}

bool CKeyStoreIsMineVisitor::operator()(const CNoDestination &dest) const
{
	return false;
}

bool CKeyStoreIsMineVisitor::operator()(const CKeyID &keyID) const
{
	return keystore->HaveKey(keyID);
}

bool CKeyStoreIsMineVisitor::operator()(const CScriptID &scriptID) const
{
	return keystore->HaveCScript(scriptID);
}

bool CKeyStoreIsMineVisitor::operator()(const CStealthAddress &stxAddr) const
{
	return stxAddr.scan_secret.size() == ec_secret_size;
}

