#include <cstring>

#include "uint/uint256.h"

uint256::uint256()
{
	for (int i = 0; i < WIDTH; i++)
		pn[i] = 0;
}

uint256::uint256(const uint_base256& b)
{
	for (int i = 0; i < WIDTH; i++)
		pn[i] = b.pn[i];
}

uint256::uint256(uint64_t b)
{
	pn[0] = (unsigned int)b;
	pn[1] = (unsigned int)(b >> 32);
	for (int i = 2; i < WIDTH; i++)
		pn[i] = 0;
}

uint256::uint256(const std::string& str)
{
	SetHex(str);
}

uint256::uint256(const std::vector<unsigned char>& vch)
{
	if (vch.size() == sizeof(pn))
		memcpy(pn, &vch[0], sizeof(pn));
	else
		*this = 0;
}

uint256& uint256::operator=(const uint_base256& b)
{
	for (int i = 0; i < WIDTH; i++)
		pn[i] = b.pn[i];
	return *this;
}

uint256& uint256::operator=(uint64_t b)
{
	pn[0] = (unsigned int)b;
	pn[1] = (unsigned int)(b >> 32);
	for (int i = 2; i < WIDTH; i++)
		pn[i] = 0;
	return *this;
}

bool operator==(const uint256& a, uint64_t b)
{
	return (uint_base256)a == b;
}

bool operator!=(const uint256& a, uint64_t b)
{
	return (uint_base256)a != b;
}

const uint256 operator<<(const uint_base256& a, unsigned int shift)
{
	return uint256(a) <<= shift;
}

const uint256 operator>>(const uint_base256& a, unsigned int shift)
{
	return uint256(a) >>= shift;
}

const uint256 operator<<(const uint256& a, unsigned int shift)
{
	return uint256(a) <<= shift;
}

const uint256 operator>>(const uint256& a, unsigned int shift)
{
	return uint256(a) >>= shift;
}

const uint256 operator^(const uint_base256& a, const uint_base256& b)
{
	return uint256(a) ^= b;
}

const uint256 operator&(const uint_base256& a, const uint_base256& b)
{
	return uint256(a) &= b;
}

const uint256 operator|(const uint_base256& a, const uint_base256& b)
{
	return uint256(a) |= b;
}

const uint256 operator+(const uint_base256& a, const uint_base256& b)
{
	return uint256(a) += b;
}

const uint256 operator-(const uint_base256& a, const uint_base256& b)
{
	return uint256(a) -= b;
}

bool operator<(const uint_base256& a, const uint256& b)
{
	return (uint_base256)a < (uint_base256)b;
}

bool operator<=(const uint_base256& a, const uint256& b)
{
	return (uint_base256)a <= (uint_base256)b;
}

bool operator>(const uint_base256& a, const uint256& b)
{
	return (uint_base256)a > (uint_base256)b;
}

bool operator>=(const uint_base256& a, const uint256& b)
{
	return (uint_base256)a >= (uint_base256)b;
}

bool operator==(const uint_base256& a, const uint256& b)
{
	return (uint_base256)a == (uint_base256)b;
}

bool operator!=(const uint_base256& a, const uint256& b)
{
	return (uint_base256)a != (uint_base256)b;
}

const uint256 operator^(const uint_base256& a, const uint256& b)
{
	return (uint_base256)a ^ (uint_base256)b;
}

const uint256 operator&(const uint_base256& a, const uint256& b)
{
	return (uint_base256)a & (uint_base256)b;
}

const uint256 operator|(const uint_base256& a, const uint256& b)
{
	return (uint_base256)a | (uint_base256)b;
}

const uint256 operator+(const uint_base256& a, const uint256& b)
{
	return (uint_base256)a + (uint_base256)b;
}

const uint256 operator-(const uint_base256& a, const uint256& b)
{
	return (uint_base256)a - (uint_base256)b;
}

bool operator<(const uint256& a, const uint_base256& b)
{
	return (uint_base256)a < (uint_base256)b;
}

bool operator<=(const uint256& a, const uint_base256& b)
{
	return (uint_base256)a <= (uint_base256)b;
}

bool operator>(const uint256& a, const uint_base256& b)
{
	return (uint_base256)a > (uint_base256)b;
}

bool operator>=(const uint256& a, const uint_base256& b)
{
	return (uint_base256)a >= (uint_base256)b;
}

bool operator==(const uint256& a, const uint_base256& b)
{
	return (uint_base256)a == (uint_base256)b;
}

bool operator!=(const uint256& a, const uint_base256& b)
{
	return (uint_base256)a != (uint_base256)b;
}

const uint256 operator^(const uint256& a, const uint_base256& b)
{
	return (uint_base256)a ^ (uint_base256)b;
}

const uint256 operator&(const uint256& a, const uint_base256& b)
{
	return (uint_base256)a & (uint_base256)b;
}

const uint256 operator|(const uint256& a, const uint_base256& b)
{
	return (uint_base256)a | (uint_base256)b;
}

const uint256 operator+(const uint256& a, const uint_base256& b)
{
	return (uint_base256)a + (uint_base256)b;
}

const uint256 operator-(const uint256& a, const uint_base256& b)
{
	return (uint_base256)a - (uint_base256)b;
}

bool operator<(const uint256& a, const uint256& b)
{
	return (uint_base256)a < (uint_base256)b;
}

bool operator<=(const uint256& a, const uint256& b)
{
	return (uint_base256)a <= (uint_base256)b;
}

bool operator>(const uint256& a, const uint256& b)
{
	return (uint_base256)a > (uint_base256)b;
}

bool operator>=(const uint256& a, const uint256& b)
{
	return (uint_base256)a >= (uint_base256)b;
}

bool operator==(const uint256& a, const uint256& b)
{
	return (uint_base256)a == (uint_base256)b;
}

bool operator!=(const uint256& a, const uint256& b)
{
	return (uint_base256)a != (uint_base256)b;
}

const uint256 operator^(const uint256& a, const uint256& b)
{
	return (uint_base256)a ^ (uint_base256)b;
}

const uint256 operator&(const uint256& a, const uint256& b)
{
	return (uint_base256)a & (uint_base256)b;
}

const uint256 operator|(const uint256& a, const uint256& b)
{
	return (uint_base256)a | (uint_base256)b;
}

const uint256 operator+(const uint256& a, const uint256& b)
{
	return (uint_base256)a + (uint_base256)b;
}

const uint256 operator-(const uint256& a, const uint256& b)
{
	return (uint_base256)a - (uint_base256)b;
}

