#include "uint256.h"
#include "serialize.h"

#include "base_uint.h"

const signed char p_util_hexdigit[256] =
{ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  0,1,2,3,4,5,6,7,8,9,-1,-1,-1,-1,-1,-1,
  -1,0xa,0xb,0xc,0xd,0xe,0xf,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,0xa,0xb,0xc,0xd,0xe,0xf,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, };


signed char HexDigit(char c)
{
    return p_util_hexdigit[(unsigned char)c];
}

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
	base_uint ret;
	for (int i = 0; i < WIDTH; i++)
		ret.pn[i] = ~pn[i];
	return ret;
}

template<unsigned int BITS>
const base_uint<BITS> base_uint<BITS>::operator-() const
{
	base_uint ret;
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



/**
	I try to patch the functionality into the cpp file but it didnt let me. In the end the project got compiled
	but the functionality didnt got into my .o file. This means on linking there was this code missing.
	
	Issue: Template class friend operators
	
	Reference:
		https://bytefreaks.net/programming-2/c/c-undefined-reference-to-templated-class-function
		http://www.c-jump.com/CIS62/L12slides/lecture.html
*/
template<unsigned int BITS>
inline bool operator<(const base_uint<BITS>& a, const base_uint<BITS>& b)
{
	for (int i = base_uint<BITS>::WIDTH-1; i >= 0; i--)
	{
		if (a.pn[i] < b.pn[i])
			return true;
		else if (a.pn[i] > b.pn[i])
			return false;
	}
	return false;
}

template<unsigned int BITS>
inline bool operator<=(const base_uint<BITS>& a, const base_uint<BITS>& b)
{
	for (int i = base_uint<BITS>::WIDTH-1; i >= 0; i--)
	{
		if (a.pn[i] < b.pn[i])
			return true;
		else if (a.pn[i] > b.pn[i])
			return false;
	}
	return true;
}

template<unsigned int BITS>
inline bool operator>(const base_uint<BITS>& a, const base_uint<BITS>& b)
{
	for (int i = base_uint<BITS>::WIDTH-1; i >= 0; i--)
	{
		if (a.pn[i] > b.pn[i])
			return true;
		else if (a.pn[i] < b.pn[i])
			return false;
	}
	return false;
}

template<unsigned int BITS>
inline bool operator>=(const base_uint<BITS>& a, const base_uint<BITS>& b)
{
	for (int i = base_uint<BITS>::WIDTH-1; i >= 0; i--)
	{
		if (a.pn[i] > b.pn[i])
			return true;
		else if (a.pn[i] < b.pn[i])
			return false;
	}
	return true;
}

template<unsigned int BITS>
inline bool operator==(const base_uint<BITS>& a, const base_uint<BITS>& b)
{
	for (int i = 0; i < base_uint<BITS>::WIDTH; i++)
		if (a.pn[i] != b.pn[i])
			return false;
	return true;
}

template<unsigned int BITS>
inline bool operator==(const base_uint<BITS>& a, uint64_t b)
{
	if (a.pn[0] != (unsigned int)b)
		return false;
	if (a.pn[1] != (unsigned int)(b >> 32))
		return false;
	for (int i = 2; i < base_uint<BITS>::WIDTH; i++)
		if (a.pn[i] != 0)
			return false;
	return true;
}

template<unsigned int BITS>
inline bool operator!=(const base_uint<BITS>& a, const base_uint<BITS>& b)
{
	return (!(a == b));
}

template<unsigned int BITS>
inline bool operator!=(const base_uint<BITS>& a, uint64_t b)
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
};

template<unsigned int BITS>
template<typename Stream>
void base_uint<BITS>::Unserialize(Stream& s, int nType, int nVersion)
{
	s.read((char*)pn, sizeof(pn));
};

/*
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
*/

/**
	Solution to force to compile the template code for different bits.
	
	Reference:
		https://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor
*/
template class base_uint<160>;
template bool operator<  <160>(const base_uint<160>& a, const base_uint<160>& b);
template bool operator<= <160>(const base_uint<160>& a, const base_uint<160>& b);
template bool operator>  <160>(const base_uint<160>& a, const base_uint<160>& b);
template bool operator>= <160>(const base_uint<160>& a, const base_uint<160>& b);
template bool operator== <160>(const base_uint<160>& a, const base_uint<160>& b);
template bool operator== <160>(const base_uint<160>& a, uint64_t b);
template bool operator!= <160>(const base_uint<160>& a, const base_uint<160>& b);
template bool operator!= <160>(const base_uint<160>& a, uint64_t b);
template void base_uint<160>::Serialize<CDataStream>(CDataStream& s, int nType, int nVersion) const;
template void base_uint<160>::Unserialize<CDataStream>(CDataStream& s, int nType, int nVersion);
template void base_uint<160>::Serialize<CAutoFile>(CAutoFile& s, int nType, int nVersion) const;
template void base_uint<160>::Unserialize<CAutoFile>(CAutoFile& s, int nType, int nVersion);

template class base_uint<256>;
template bool operator<  <256>(const base_uint<256>& a, const base_uint<256>& b);
template bool operator<= <256>(const base_uint<256>& a, const base_uint<256>& b);
template bool operator>  <256>(const base_uint<256>& a, const base_uint<256>& b);
template bool operator>= <256>(const base_uint<256>& a, const base_uint<256>& b);
template bool operator== <256>(const base_uint<256>& a, const base_uint<256>& b);
template bool operator== <256>(const base_uint<256>& a, uint64_t b);
template bool operator!= <256>(const base_uint<256>& a, const base_uint<256>& b);
template bool operator!= <256>(const base_uint<256>& a, uint64_t b);
template void base_uint<256>::Serialize<CDataStream>(CDataStream& s, int nType, int nVersion) const;
template void base_uint<256>::Unserialize<CDataStream>(CDataStream& s, int nType, int nVersion);
template void base_uint<256>::Serialize<CAutoFile>(CAutoFile& s, int nType, int nVersion) const;
template void base_uint<256>::Unserialize<CAutoFile>(CAutoFile& s, int nType, int nVersion);

template class base_uint<512>;
template bool operator<  <512>(const base_uint<512>& a, const base_uint<512>& b);
template bool operator<= <512>(const base_uint<512>& a, const base_uint<512>& b);
template bool operator>  <512>(const base_uint<512>& a, const base_uint<512>& b);
template bool operator>= <512>(const base_uint<512>& a, const base_uint<512>& b);
template bool operator== <512>(const base_uint<512>& a, const base_uint<512>& b);
template bool operator== <512>(const base_uint<512>& a, uint64_t b);
template bool operator!= <512>(const base_uint<512>& a, const base_uint<512>& b);
template bool operator!= <512>(const base_uint<512>& a, uint64_t b);
template void base_uint<512>::Serialize<CDataStream>(CDataStream& s, int nType, int nVersion) const;
template void base_uint<512>::Unserialize<CDataStream>(CDataStream& s, int nType, int nVersion);
template void base_uint<512>::Serialize<CAutoFile>(CAutoFile& s, int nType, int nVersion) const;
template void base_uint<512>::Unserialize<CAutoFile>(CAutoFile& s, int nType, int nVersion);
