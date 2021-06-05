#ifndef CDIGITALNOTEADDRESSVISITOR_H
#define CDIGITALNOTEADDRESSVISITOR_H

#include <boost/variant/static_visitor.hpp>

class CDigitalNoteAddress;
class CKeyID;
class CScriptID;
class CNoDestination;
class CStealthAddress;

class CDigitalNoteAddressVisitor : public boost::static_visitor<bool>
{
private:
	CDigitalNoteAddress* addr;

public:
	CDigitalNoteAddressVisitor(CDigitalNoteAddress* addrIn);

	bool operator()(const CKeyID &id) const;
	bool operator()(const CScriptID &id) const;
	bool operator()(const CNoDestination &no) const;
	bool operator()(const CStealthAddress &stxAddr) const;
};

#endif // CDIGITALNOTEADDRESSVISITOR_H
