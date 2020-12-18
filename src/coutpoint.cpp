#include "tinyformat.h"

#include "coutpoint.h"

COutPoint::COutPoint()
{
	SetNull();
}

COutPoint::COutPoint(uint256 hashIn, unsigned int nIn)
{
	hash = hashIn;
	n = nIn;
}

void COutPoint::SetNull()
{
	hash = 0;
	n = (unsigned int) - 1;
}

bool COutPoint::IsNull() const
{
	return (hash == 0 && n == (unsigned int) -1);
}

bool operator<(const COutPoint& a, const COutPoint& b)
{
	return (a.hash < b.hash || (a.hash == b.hash && a.n < b.n));
}

bool operator==(const COutPoint& a, const COutPoint& b)
{
	return (a.hash == b.hash && a.n == b.n);
}

bool operator!=(const COutPoint& a, const COutPoint& b)
{
	return !(a == b);
}

std::string COutPoint::ToString() const
{
    return strprintf("COutPoint(%s, %u)", hash.ToString().substr(0,10), n);
}

std::string COutPoint::ToStringShort() const
{
    return strprintf("%s-%u", hash.ToString().substr(0,64), n);
}

