#include "util.h"

#include "ctxin.h"

CTxIn::CTxIn()
{
	nSequence = std::numeric_limits<unsigned int>::max();
}

CTxIn::CTxIn(COutPoint prevoutIn, CScript scriptSigIn, unsigned int nSequenceIn)
{
    prevout = prevoutIn;
    scriptSig = scriptSigIn;
    nSequence = nSequenceIn;
}

CTxIn::CTxIn(uint256 hashPrevTx, unsigned int nOut, CScript scriptSigIn, unsigned int nSequenceIn)
{
    prevout = COutPoint(hashPrevTx, nOut);
    scriptSig = scriptSigIn;
    nSequence = nSequenceIn;
}

bool CTxIn::IsFinal() const
{
	return (nSequence == std::numeric_limits<unsigned int>::max());
}

bool operator==(const CTxIn& a, const CTxIn& b)
{
	return (a.prevout   == b.prevout &&
			a.scriptSig == b.scriptSig &&
			a.nSequence == b.nSequence);
}

bool operator!=(const CTxIn& a, const CTxIn& b)
{
	return !(a == b);
}

std::string CTxIn::ToString() const
{
    std::string str;
    str += "CTxIn(";
    str += prevout.ToString();
    if (prevout.IsNull())
        str += strprintf(", coinbase %s", HexStr(scriptSig));
    else
        str += strprintf(", scriptSig=%s", scriptSig.ToString().substr(0,24));
    if (nSequence != std::numeric_limits<unsigned int>::max())
        str += strprintf(", nSequence=%u", nSequence);
    str += ")";
    return str;
}

