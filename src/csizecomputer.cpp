#include "serialize.h"

#include "csizecomputer.h"

CSizeComputer::CSizeComputer(int nTypeIn, int nVersionIn) : nSize(0), nType(nTypeIn), nVersion(nVersionIn)
{
	
}

CSizeComputer& CSizeComputer::write(const char *psz, size_t nSize)
{
	this->nSize += nSize;
	
	return *this;
}

template<typename T>
CSizeComputer& CSizeComputer::operator<<(const T& obj)
{
	::Serialize(*this, obj, nType, nVersion);
	
	return (*this);
}

size_t CSizeComputer::size() const
{
	return nSize;
}

