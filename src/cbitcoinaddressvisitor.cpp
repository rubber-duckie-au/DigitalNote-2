#include "cbitcoinaddress.h"
#include "cnodestination.h"
#include "ckeyid.h"
#include "cscriptid.h"
#include "cstealthaddress.h"

#include "cbitcoinaddressvisitor.h"

CBitcoinAddressVisitor::CBitcoinAddressVisitor(CBitcoinAddress *addrIn) : addr(addrIn)
{
	
}

bool CBitcoinAddressVisitor::operator()(const CKeyID &id) const
{
	return addr->Set(id);
}

bool CBitcoinAddressVisitor::operator()(const CScriptID &id) const
{
	return addr->Set(id);
}

bool CBitcoinAddressVisitor::operator()(const CNoDestination &no) const
{
	return false;
}

bool CBitcoinAddressVisitor::operator()(const CStealthAddress &stxAddr) const
{
	return false;
}

