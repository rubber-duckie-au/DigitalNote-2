#ifndef CBITCOINADDRESSVISITOR_H
#define CBITCOINADDRESSVISITOR_H

#include <boost/variant/static_visitor.hpp>

class CBitcoinAddress;
class CKeyID;
class CScriptID;
class CNoDestination;
class CStealthAddress;

class CBitcoinAddressVisitor : public boost::static_visitor<bool>
{
private:
	CBitcoinAddress* addr;
	
public:
	CBitcoinAddressVisitor(CBitcoinAddress* addrIn);
	bool operator()(const CKeyID &id) const;
	bool operator()(const CScriptID &id) const;
	bool operator()(const CNoDestination &no) const;
	bool operator()(const CStealthAddress &stxAddr) const;
};

#endif // CBITCOINADDRESSVISITOR_H
