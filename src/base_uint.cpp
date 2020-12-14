#include "uint256.h"
#include "utilstrencodings.h"

#include "base_uint.h"

template<unsigned int BITS>
bool base_uint<BITS>::operator!() const
{
	for (int i = 0; i < WIDTH; i++)
		if (pn[i] != 0)
			return false;
	return true;
}

template<unsigned int BITS>
const base_uint<BITS> base_uint<BITS>::operator~() const
{
	base_uint<BITS> ret;
	for (int i = 0; i < WIDTH; i++)
		ret.pn[i] = ~pn[i];
	return ret;
}

template<unsigned int BITS>
const base_uint<BITS> base_uint<BITS>::operator-() const
{
	base_uint<BITS> ret;
	for (int i = 0; i < WIDTH; i++)
		ret.pn[i] = ~pn[i];
	ret++;
	return ret;
}

template<unsigned int BITS>
double base_uint<BITS>::getdouble() const
{
	double ret = 0.0;
	double fact = 1.0;
	for (int i = 0; i < WIDTH; i++) {
		ret += fact * pn[i];
		fact *= 4294967296.0;
	}
	return ret;
}

template<unsigned int BITS>
base_uint<BITS>& base_uint<BITS>::operator=(uint64_t b)
{
	pn[0] = (unsigned int)b;
	pn[1] = (unsigned int)(b >> 32);
	for (int i = 2; i < WIDTH; i++)
		pn[i] = 0;
	return *this;
}

template<unsigned int BITS>
base_uint<BITS>& base_uint<BITS>::operator^=(const base_uint<BITS>& b)
{
	for (int i = 0; i < WIDTH; i++)
		pn[i] ^= b.pn[i];
	return *this;
}

template<unsigned int BITS>
base_uint<BITS>& base_uint<BITS>::operator&=(const base_uint<BITS>& b)
{
	for (int i = 0; i < WIDTH; i++)
		pn[i] &= b.pn[i];
	return *this;
}

template<unsigned int BITS>
base_uint<BITS>& base_uint<BITS>::operator|=(const base_uint<BITS>& b)
{
	for (int i = 0; i < WIDTH; i++)
		pn[i] |= b.pn[i];
	return *this;
}

template<unsigned int BITS>
base_uint<BITS>& base_uint<BITS>::operator^=(uint64_t b)
{
	pn[0] ^= (unsigned int)b;
	pn[1] ^= (unsigned int)(b >> 32);
	return *this;
}

template<unsigned int BITS>
base_uint<BITS>& base_uint<BITS>::operator|=(uint64_t b)
{
	pn[0] |= (unsigned int)b;
	pn[1] |= (unsigned int)(b >> 32);
	return *this;
}

template<unsigned int BITS>
base_uint<BITS>& base_uint<BITS>::operator<<=(unsigned int shift)
{
	base_uint a(*this);
	for (int i = 0; i < WIDTH; i++)
		pn[i] = 0;
	int k = shift / 32;
	shift = shift % 32;
	for (int i = 0; i < WIDTH; i++)
	{
		if (i+k+1 < WIDTH && shift != 0)
			pn[i+k+1] |= (a.pn[i] >> (32-shift));
		if (i+k < WIDTH)
			pn[i+k] |= (a.pn[i] << shift);
	}
	return *this;
}

template<unsigned int BITS>
base_uint<BITS>& base_uint<BITS>::operator>>=(unsigned int shift)
{
	base_uint a(*this);
	for (int i = 0; i < WIDTH; i++)
		pn[i] = 0;
	int k = shift / 32;
	shift = shift % 32;
	for (int i = 0; i < WIDTH; i++)
	{
		if (i-k-1 >= 0 && shift != 0)
			pn[i-k-1] |= (a.pn[i] << (32-shift));
		if (i-k >= 0)
			pn[i-k] |= (a.pn[i] >> shift);
	}
	return *this;
}

template<unsigned int BITS>
base_uint<BITS>& base_uint<BITS>::operator+=(const base_uint<BITS>& b)
{
	uint64_t carry = 0;
	for (int i = 0; i < WIDTH; i++)
	{
		uint64_t n = carry + pn[i] + b.pn[i];
		pn[i] = n & 0xffffffff;
		carry = n >> 32;
	}
	return *this;
}

template<unsigned int BITS>
base_uint<BITS>& base_uint<BITS>::operator-=(const base_uint<BITS>& b)
{
	*this += -b;
	return *this;
}

template<unsigned int BITS>
base_uint<BITS>& base_uint<BITS>::operator+=(uint64_t b64)
{
	base_uint b;
	b = b64;
	*this += b;
	return *this;
}

template<unsigned int BITS>
base_uint<BITS>& base_uint<BITS>::operator-=(uint64_t b64)
{
	base_uint b;
	b = b64;
	*this += -b;
	return *this;
}

template<unsigned int BITS>
base_uint<BITS>& base_uint<BITS>::operator++()
{
	// prefix operator
	int i = 0;
	while (++pn[i] == 0 && i < WIDTH-1)
		i++;
	return *this;
}

template<unsigned int BITS>
const base_uint<BITS> base_uint<BITS>::operator++(int)
{
	// postfix operator
	const base_uint ret = *this;
	++(*this);
	return ret;
}

template<unsigned int BITS>
base_uint<BITS>& base_uint<BITS>::operator--()
{
	// prefix operator
	int i = 0;
	while (--pn[i] == -1 && i < WIDTH-1)
		i++;
	return *this;
}

template<unsigned int BITS>
const base_uint<BITS> base_uint<BITS>::operator--(int)
{
	// postfix operator
	const base_uint ret = *this;
	--(*this);
	return ret;
}

template<unsigned int _BITS_>
bool operator<(const base_uint<_BITS_>& a, const base_uint<_BITS_>& b)
{
	for (int i = base_uint<_BITS_>::WIDTH-1; i >= 0; i--)
	{
		if (a.pn[i] < b.pn[i])
			return true;
		else if (a.pn[i] > b.pn[i])
			return false;
	}
	return false;
}

template<unsigned int _BITS_>
bool operator<=(const base_uint<_BITS_>& a, const base_uint<_BITS_>& b)
{
	for (int i = base_uint<_BITS_>::WIDTH-1; i >= 0; i--)
	{
		if (a.pn[i] < b.pn[i])
			return true;
		else if (a.pn[i] > b.pn[i])
			return false;
	}
	return true;
}

template<unsigned int _BITS_>
bool operator>(const base_uint<_BITS_>& a, const base_uint<_BITS_>& b)
{
	for (int i = base_uint<_BITS_>::WIDTH-1; i >= 0; i--)
	{
		if (a.pn[i] > b.pn[i])
			return true;
		else if (a.pn[i] < b.pn[i])
			return false;
	}
	return false;
}

template<unsigned int _BITS_>
bool operator>=(const base_uint<_BITS_>& a, const base_uint<_BITS_>& b)
{
	for (int i = base_uint<_BITS_>::WIDTH-1; i >= 0; i--)
	{
		if (a.pn[i] > b.pn[i])
			return true;
		else if (a.pn[i] < b.pn[i])
			return false;
	}
	return true;
}

template<unsigned int _BITS_>
bool operator==(const base_uint<_BITS_>& a, const base_uint<_BITS_>& b)
{
	for (int i = 0; i < base_uint<_BITS_>::WIDTH; i++)
		if (a.pn[i] != b.pn[i])
			return false;
	return true;
}

template<unsigned int _BITS_>
bool operator==(const base_uint<_BITS_>& a, uint64_t b)
{
	if (a.pn[0] != (unsigned int)b)
		return false;
	if (a.pn[1] != (unsigned int)(b >> 32))
		return false;
	for (int i = 2; i < base_uint<_BITS_>::WIDTH; i++)
		if (a.pn[i] != 0)
			return false;
	return true;
}

template<unsigned int _BITS_>
bool operator!=(const base_uint<_BITS_>& a, const base_uint<_BITS_>& b)
{
	return (!(a == b));
}

template<unsigned int _BITS_>
bool operator!=(const base_uint<_BITS_>& a, uint64_t b)
{
	return (!(a == b));
}

template<unsigned int BITS>
std::string base_uint<BITS>::GetHex() const
{
	char psz[sizeof(pn)*2 + 1];
	for (unsigned int i = 0; i < sizeof(pn); i++)
		sprintf(psz + i*2, "%02x", ((unsigned char*)pn)[sizeof(pn) - i - 1]);
	return std::string(psz, psz + sizeof(pn)*2);
}

template<unsigned int BITS>
void base_uint<BITS>::SetHex(const char* psz)
{
	memset(pn,0,sizeof(pn));

	// skip leading spaces
	while (isspace(*psz))
		psz++;

	// skip 0x
	if (psz[0] == '0' && tolower(psz[1]) == 'x')
		psz += 2;

	// hex string to uint
	const char* pbegin = psz;
	while (::HexDigit(*psz) != -1)
		psz++;
	psz--;
	unsigned char* p1 = (unsigned char*)pn;
	unsigned char* pend = p1 + WIDTH * 4;
	while (psz >= pbegin && p1 < pend)
	{
		*p1 = ::HexDigit(*psz--);
		if (psz >= pbegin)
		{
			*p1 |= ((unsigned char)::HexDigit(*psz--) << 4);
			p1++;
		}
	}
}

template<unsigned int BITS>
void base_uint<BITS>::SetHex(const std::string& str)
{
	SetHex(str.c_str());
}

template<unsigned int BITS>
std::string base_uint<BITS>::ToString() const
{
	return (GetHex());
}

template<unsigned int BITS>
unsigned char* base_uint<BITS>::begin()
{
	return (unsigned char*)&pn[0];
}

template<unsigned int BITS>
unsigned char* base_uint<BITS>::end()
{
	return (unsigned char*)&pn[WIDTH];
}

template<unsigned int BITS>
const unsigned char* base_uint<BITS>::begin() const
{
return (unsigned char*)&pn[0];
}

template<unsigned int BITS>
const unsigned char* base_uint<BITS>::end() const
{
return (unsigned char*)&pn[WIDTH];
}

template<unsigned int BITS>
unsigned int base_uint<BITS>::size()
{
	return sizeof(pn);
}

template<unsigned int BITS>
uint64_t base_uint<BITS>::Get64(int n) const
{
	return pn[2*n] | (uint64_t)pn[2*n+1] << 32;
}

template<unsigned int BITS>
unsigned int base_uint<BITS>::GetSerializeSize(int nType, int nVersion) const
{
	return sizeof(pn);
}

template<unsigned int BITS>
template<typename Stream>
void base_uint<BITS>::Serialize(Stream& s, int nType, int nVersion) const
{
	s.write((char*)pn, sizeof(pn));
}

template<unsigned int BITS>
template<typename Stream>
void base_uint<BITS>::Unserialize(Stream& s, int nType, int nVersion)
{
	s.read((char*)pn, sizeof(pn));
}

