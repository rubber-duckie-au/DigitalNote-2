#include <cstring>

#include "uint/uint160.h"

uint160::uint160()
{
	for (int i = 0; i < WIDTH; i++)
		pn[i] = 0;
}

uint160::uint160(const uint_base160& b)
{
	for (int i = 0; i < WIDTH; i++)
		pn[i] = b.pn[i];
}

uint160::uint160(uint64_t b)
{
	pn[0] = (unsigned int)b;
	pn[1] = (unsigned int)(b >> 32);
	for (int i = 2; i < WIDTH; i++)
		pn[i] = 0;
}

uint160::uint160(const std::string& str)
{
	SetHex(str);
}

uint160::uint160(const std::vector<unsigned char>& vch)
{
	if (vch.size() == sizeof(pn))
		memcpy(pn, &vch[0], sizeof(pn));
	else
		*this = 0;
}

uint160& uint160::operator=(const uint_base160& b)
{
	for (int i = 0; i < WIDTH; i++)
		pn[i] = b.pn[i];
	return *this;
}

uint160& uint160::operator=(uint64_t b)
{
	pn[0] = (unsigned int)b;
	pn[1] = (unsigned int)(b >> 32);
	for (int i = 2; i < WIDTH; i++)
		pn[i] = 0;
	return *this;
}

bool operator==(const uint160& a, uint64_t b)
{
	return (uint_base160)a == b;
}

bool operator!=(const uint160& a, uint64_t b)
{
	return (uint_base160)a != b;
}

const uint160 operator<<(const uint_base160& a, unsigned int shift)
{
	return uint160(a) <<= shift;
}

const uint160 operator>>(const uint_base160& a, unsigned int shift)
{
	return uint160(a) >>= shift;
}

const uint160 operator<<(const uint160& a, unsigned int shift)
{
	return uint160(a) <<= shift;
}

const uint160 operator>>(const uint160& a, unsigned int shift)
{
	return uint160(a) >>= shift;
}

const uint160 operator^(const uint_base160& a, const uint_base160& b)
{
	return uint160(a) ^= b;
}

const uint160 operator&(const uint_base160& a, const uint_base160& b)
{
	return uint160(a) &= b;
}

const uint160 operator|(const uint_base160& a, const uint_base160& b)
{
	return uint160(a) |= b;
}

const uint160 operator+(const uint_base160& a, const uint_base160& b)
{
	return uint160(a) += b;
}

const uint160 operator-(const uint_base160& a, const uint_base160& b)
{
	return uint160(a) -= b;
}

bool operator<(const uint_base160& a, const uint160& b)
{
	return (uint_base160)a < (uint_base160)b;
}

bool operator<=(const uint_base160& a, const uint160& b)
{
	return (uint_base160)a <= (uint_base160)b;
}

bool operator>(const uint_base160& a, const uint160& b)
{
	return (uint_base160)a > (uint_base160)b;
}

bool operator>=(const uint_base160& a, const uint160& b)
{
	return (uint_base160)a >= (uint_base160)b;
}

bool operator==(const uint_base160& a, const uint160& b)
{
	return (uint_base160)a == (uint_base160)b;
}

bool operator!=(const uint_base160& a, const uint160& b)
{
	return (uint_base160)a != (uint_base160)b;
}

const uint160 operator^(const uint_base160& a, const uint160& b)
{
	return (uint_base160)a ^ (uint_base160)b;
}

const uint160 operator&(const uint_base160& a, const uint160& b)
{
	return (uint_base160)a & (uint_base160)b;
}

const uint160 operator|(const uint_base160& a, const uint160& b)
{
	return (uint_base160)a | (uint_base160)b;
}

const uint160 operator+(const uint_base160& a, const uint160& b)
{
	return (uint_base160)a + (uint_base160)b;
}

const uint160 operator-(const uint_base160& a, const uint160& b)
{
	return (uint_base160)a - (uint_base160)b;
}

bool operator<(const uint160& a, const uint_base160& b)
{
	return (uint_base160)a < (uint_base160)b;
}

bool operator<=(const uint160& a, const uint_base160& b)
{
	return (uint_base160)a <= (uint_base160)b;
}

bool operator>(const uint160& a, const uint_base160& b)
{
	return (uint_base160)a > (uint_base160)b;
}

bool operator>=(const uint160& a, const uint_base160& b)
{
	return (uint_base160)a >= (uint_base160)b;
}

bool operator==(const uint160& a, const uint_base160& b)
{
	return (uint_base160)a == (uint_base160)b;
}

bool operator!=(const uint160& a, const uint_base160& b)
{
	return (uint_base160)a != (uint_base160)b;
}

const uint160 operator^(const uint160& a, const uint_base160& b)
{
	return (uint_base160)a ^ (uint_base160)b;
}

const uint160 operator&(const uint160& a, const uint_base160& b)
{
	return (uint_base160)a & (uint_base160)b;
}

const uint160 operator|(const uint160& a, const uint_base160& b)
{
	return (uint_base160)a | (uint_base160)b;
}

const uint160 operator+(const uint160& a, const uint_base160& b)
{
	return (uint_base160)a + (uint_base160)b;
}

const uint160 operator-(const uint160& a, const uint_base160& b)
{
	return (uint_base160)a - (uint_base160)b;
}

bool operator<(const uint160& a, const uint160& b)
{
	return (uint_base160)a < (uint_base160)b;
}

bool operator<=(const uint160& a, const uint160& b)
{
	return (uint_base160)a <= (uint_base160)b;
}

bool operator>(const uint160& a, const uint160& b)
{
	return (uint_base160)a > (uint_base160)b;
}

bool operator>=(const uint160& a, const uint160& b)
{
	return (uint_base160)a >= (uint_base160)b;
}

bool operator==(const uint160& a, const uint160& b)
{
	return (uint_base160)a == (uint_base160)b;
}

bool operator!=(const uint160& a, const uint160& b)
{
	return (uint_base160)a != (uint_base160)b;
}

const uint160 operator^(const uint160& a, const uint160& b)
{
	return (uint_base160)a ^ (uint_base160)b;
}

const uint160 operator&(const uint160& a, const uint160& b)
{
	return (uint_base160)a & (uint_base160)b;
}

const uint160 operator|(const uint160& a, const uint160& b)
{
	return (uint_base160)a | (uint_base160)b;
}

const uint160 operator+(const uint160& a, const uint160& b)
{
	return (uint_base160)a + (uint_base160)b;
}

const uint160 operator-(const uint160& a, const uint160& b)
{
	return (uint_base160)a - (uint_base160)b;
}

