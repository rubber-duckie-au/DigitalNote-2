#ifndef UINT_BASE_H
#define UINT_BASE_H

#include <cstdint>
#include <string>

// Forward declare class template
template<unsigned int BITS>
class uint_base;

// Forward declare function template
template<unsigned int BITS>
bool operator<(const uint_base<BITS>& a, const uint_base<BITS>& b);
template<unsigned int BITS>
bool operator<=(const uint_base<BITS>& a, const uint_base<BITS>& b);
template<unsigned int BITS>
bool operator>(const uint_base<BITS>& a, const uint_base<BITS>& b);
template<unsigned int BITS>
bool operator>=(const uint_base<BITS>& a, const uint_base<BITS>& b);
template<unsigned int BITS>
bool operator==(const uint_base<BITS>& a, const uint_base<BITS>& b);
template<unsigned int BITS>
bool operator==(const uint_base<BITS>& a, uint64_t b);
template<unsigned int BITS>
bool operator!=(const uint_base<BITS>& a, const uint_base<BITS>& b);
template<unsigned int BITS>
bool operator!=(const uint_base<BITS>& a, uint64_t b);

/** Base class without constructors for uint256 and uint160.
 * This makes the compiler let u use it in a union.
 */
template<unsigned int BITS>
class uint_base
{
protected:
	enum
	{
		WIDTH=BITS/32
	};
	unsigned int pn[WIDTH];

public:
	bool operator!() const;
	const uint_base operator~() const;
	const uint_base operator-() const;
	double getdouble() const;
	uint_base& operator=(uint64_t b);
	uint_base& operator^=(const uint_base& b);
	uint_base& operator&=(const uint_base& b);
	uint_base& operator|=(const uint_base& b);
	uint_base& operator^=(uint64_t b);
	uint_base& operator|=(uint64_t b);
	uint_base& operator<<=(unsigned int shift);
	uint_base& operator>>=(unsigned int shift);
	uint_base& operator+=(const uint_base& b);
	uint_base& operator-=(const uint_base& b);
	uint_base& operator+=(uint64_t b64);
	uint_base& operator-=(uint64_t b64);
	uint_base& operator++();
	const uint_base operator++(int);
	uint_base& operator--();
	const uint_base operator--(int);
	std::string GetHex() const;
	void SetHex(const char* psz);
	void SetHex(const std::string& str);
	std::string ToString() const;
	unsigned char* begin();
	unsigned char* end();
	const unsigned char* begin() const;
	const unsigned char* end() const;
	unsigned int size();
	uint64_t Get64(int n=0) const;
	unsigned int GetSerializeSize(int nType, int nVersion) const;
	template<typename Stream>
	void Serialize(Stream& s, int nType, int nVersion) const;
	template<typename Stream>
	void Unserialize(Stream& s, int nType, int nVersion);

	// Friends to give access to protected variables
	friend class uint160;
	friend class uint256;
	friend class uint512;

	friend bool operator<  <>(const uint_base<BITS>& a, const uint_base<BITS>& b);
	friend bool operator<= <>(const uint_base<BITS>& a, const uint_base<BITS>& b);
	friend bool operator>  <>(const uint_base<BITS>& a, const uint_base<BITS>& b);
	friend bool operator>= <>(const uint_base<BITS>& a, const uint_base<BITS>& b);
	friend bool operator== <>(const uint_base<BITS>& a, const uint_base<BITS>& b);
	friend bool operator== <>(const uint_base<BITS>& a, uint64_t b);
	friend bool operator!= <>(const uint_base<BITS>& a, const uint_base<BITS>& b);
	friend bool operator!= <>(const uint_base<BITS>& a, uint64_t b);
};

typedef uint_base<160> uint_base160;
typedef uint_base<256> uint_base256;
typedef uint_base<512> uint_base512;

#endif // UINT_BASE_H
