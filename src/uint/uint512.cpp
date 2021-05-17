#include <cstring>
#include "uint/uint256.h"

#include "uint/uint512.h"

uint512::uint512()
{
	for (int i = 0; i < WIDTH; i++)
		pn[i] = 0;
}

uint512::uint512(const uint_base512& b)
{
	for (int i = 0; i < WIDTH; i++)
		pn[i] = b.pn[i];
}

uint512::uint512(uint64_t b)
{
	pn[0] = (unsigned int)b;
	pn[1] = (unsigned int)(b >> 32);
	for (int i = 2; i < WIDTH; i++)
		pn[i] = 0;
}

uint512::uint512(const std::string& str)
{
	SetHex(str);
}

uint512::uint512(const std::vector<unsigned char>& vch)
{
	if (vch.size() == sizeof(pn))
		memcpy(pn, &vch[0], sizeof(pn));
	else
		*this = 0;
}

uint512& uint512::operator=(const uint_base512& b)
{
	for (int i = 0; i < WIDTH; i++)
		pn[i] = b.pn[i];
	return *this;
}

uint512& uint512::operator=(uint64_t b)
{
	pn[0] = (unsigned int)b;
	pn[1] = (unsigned int)(b >> 32);
	for (int i = 2; i < WIDTH; i++)
		pn[i] = 0;
	return *this;
}

uint256 uint512::trim256() const
{
	uint256 ret;
	for (unsigned int i = 0; i < uint256::WIDTH; i++){
		ret.pn[i] = pn[i];
	}
	return ret;
}

bool operator==(const uint512& a, uint64_t b)
{
	return (uint_base512)a == b;
}

bool operator!=(const uint512& a, uint64_t b)
{
	return (uint_base512)a != b;
}

const uint512 operator<<(const uint_base512& a, unsigned int shift)
{
	return uint512(a) <<= shift;
}

const uint512 operator>>(const uint_base512& a, unsigned int shift)
{
	return uint512(a) >>= shift;
}

const uint512 operator<<(const uint512& a, unsigned int shift)
{
	return uint512(a) <<= shift;
}

const uint512 operator>>(const uint512& a, unsigned int shift)
{
	return uint512(a) >>= shift;
}

const uint512 operator^(const uint_base512& a, const uint_base512& b)
{
	return uint512(a) ^= b;
}

const uint512 operator&(const uint_base512& a, const uint_base512& b)
{
	return uint512(a) &= b;
}

const uint512 operator|(const uint_base512& a, const uint_base512& b)
{
	return uint512(a) |= b;
}

const uint512 operator+(const uint_base512& a, const uint_base512& b)
{
	return uint512(a) += b;
}

const uint512 operator-(const uint_base512& a, const uint_base512& b)
{
	return uint512(a) -= b;
}

bool operator<(const uint_base512& a, const uint512& b)
{
	return (uint_base512)a < (uint_base512)b;
}

bool operator<=(const uint_base512& a, const uint512& b)
{
	return (uint_base512)a <= (uint_base512)b;
}

bool operator>(const uint_base512& a, const uint512& b)
{
	return (uint_base512)a > (uint_base512)b;
}

bool operator>=(const uint_base512& a, const uint512& b)
{
	return (uint_base512)a >= (uint_base512)b;
}

bool operator==(const uint_base512& a, const uint512& b)
{
	return (uint_base512)a == (uint_base512)b;
}

bool operator!=(const uint_base512& a, const uint512& b)
{
	return (uint_base512)a != (uint_base512)b;
}

const uint512 operator^(const uint_base512& a, const uint512& b)
{
	return (uint_base512)a ^ (uint_base512)b;
}

const uint512 operator&(const uint_base512& a, const uint512& b)
{
	return (uint_base512)a & (uint_base512)b;
}

const uint512 operator|(const uint_base512& a, const uint512& b)
{
	return (uint_base512)a | (uint_base512)b;
}

const uint512 operator+(const uint_base512& a, const uint512& b)
{
	return (uint_base512)a + (uint_base512)b;
}

const uint512 operator-(const uint_base512& a, const uint512& b)
{
	return (uint_base512)a - (uint_base512)b;
}

bool operator<(const uint512& a, const uint_base512& b)
{
	return (uint_base512)a < (uint_base512)b;
}

bool operator<=(const uint512& a, const uint_base512& b)
{
	return (uint_base512)a <= (uint_base512)b;
}

bool operator>(const uint512& a, const uint_base512& b)
{
	return (uint_base512)a > (uint_base512)b;
}

bool operator>=(const uint512& a, const uint_base512& b)
{
	return (uint_base512)a >= (uint_base512)b;
}

bool operator==(const uint512& a, const uint_base512& b)
{
	return (uint_base512)a == (uint_base512)b;
}

bool operator!=(const uint512& a, const uint_base512& b)
{
	return (uint_base512)a != (uint_base512)b;
}

const uint512 operator^(const uint512& a, const uint_base512& b)
{
	return (uint_base512)a ^ (uint_base512)b;
}

const uint512 operator&(const uint512& a, const uint_base512& b)
{
	return (uint_base512)a & (uint_base512)b;
}

const uint512 operator|(const uint512& a, const uint_base512& b)
{
	return (uint_base512)a | (uint_base512)b;
}

const uint512 operator+(const uint512& a, const uint_base512& b)
{
	return (uint_base512)a + (uint_base512)b;
}

const uint512 operator-(const uint512& a, const uint_base512& b)
{
	return (uint_base512)a - (uint_base512)b;
}

bool operator<(const uint512& a, const uint512& b)
{
	return (uint_base512)a < (uint_base512)b;
}

bool operator<=(const uint512& a, const uint512& b)
{
	return (uint_base512)a <= (uint_base512)b;
}

bool operator>(const uint512& a, const uint512& b)
{
	return (uint_base512)a > (uint_base512)b;
}

bool operator>=(const uint512& a, const uint512& b)
{
	return (uint_base512)a >= (uint_base512)b;
}

bool operator==(const uint512& a, const uint512& b)
{
	return (uint_base512)a == (uint_base512)b;
}

bool operator!=(const uint512& a, const uint512& b)
{
	return (uint_base512)a != (uint_base512)b;
}

const uint512 operator^(const uint512& a, const uint512& b)
{
	return (uint_base512)a ^ (uint_base512)b;
}

const uint512 operator&(const uint512& a, const uint512& b)
{
	return (uint_base512)a & (uint_base512)b;
}

const uint512 operator|(const uint512& a, const uint512& b)
{
	return (uint_base512)a | (uint_base512)b;
}

const uint512 operator+(const uint512& a, const uint512& b)
{
	return (uint_base512)a + (uint_base512)b;
}

const uint512 operator-(const uint512& a, const uint512& b)
{
	return (uint_base512)a - (uint_base512)b;
}

