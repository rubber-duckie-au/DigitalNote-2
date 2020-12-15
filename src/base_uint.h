#ifndef BASE_UINT_H
#define BASE_UINT_H

#include <ostream>

extern const signed char p_util_hexdigit[256];

signed char HexDigit(char c);

// Forward declare class template
template<unsigned int BITS>
class base_uint;

// Forward declare function template
template<unsigned int BITS>
bool operator<(const base_uint<BITS>& a, const base_uint<BITS>& b);
template<unsigned int BITS>
bool operator<=(const base_uint<BITS>& a, const base_uint<BITS>& b);
template<unsigned int BITS>
bool operator>(const base_uint<BITS>& a, const base_uint<BITS>& b);
template<unsigned int BITS>
bool operator>=(const base_uint<BITS>& a, const base_uint<BITS>& b);
template<unsigned int BITS>
bool operator==(const base_uint<BITS>& a, const base_uint<BITS>& b);
template<unsigned int BITS>
bool operator==(const base_uint<BITS>& a, uint64_t b);
template<unsigned int BITS>
bool operator!=(const base_uint<BITS>& a, const base_uint<BITS>& b);
template<unsigned int BITS>
bool operator!=(const base_uint<BITS>& a, uint64_t b);

/** Base class without constructors for uint256 and uint160.
 * This makes the compiler let u use it in a union.
 */
template<unsigned int BITS>
class base_uint
{
public:
    enum
	{
		WIDTH=BITS/32
	};
    unsigned int pn[WIDTH];
	
    bool operator!() const;
    const base_uint operator~() const;
    const base_uint operator-() const;
    double getdouble() const;
    base_uint& operator=(uint64_t b);
    base_uint& operator^=(const base_uint& b);
    base_uint& operator&=(const base_uint& b);
    base_uint& operator|=(const base_uint& b);
    base_uint& operator^=(uint64_t b);
    base_uint& operator|=(uint64_t b);
    base_uint& operator<<=(unsigned int shift);
    base_uint& operator>>=(unsigned int shift);
    base_uint& operator+=(const base_uint& b);
    base_uint& operator-=(const base_uint& b);
    base_uint& operator+=(uint64_t b64);
    base_uint& operator-=(uint64_t b64);
    base_uint& operator++();
    const base_uint operator++(int);
    base_uint& operator--();
    const base_uint operator--(int);
	
	friend bool operator<  <>(const base_uint<BITS>& a, const base_uint<BITS>& b);
	friend bool operator<= <>(const base_uint<BITS>& a, const base_uint<BITS>& b);
	friend bool operator>  <>(const base_uint<BITS>& a, const base_uint<BITS>& b);
	friend bool operator>= <>(const base_uint<BITS>& a, const base_uint<BITS>& b);
	friend bool operator== <>(const base_uint<BITS>& a, const base_uint<BITS>& b);
	friend bool operator== <>(const base_uint<BITS>& a, uint64_t b);
	friend bool operator!= <>(const base_uint<BITS>& a, const base_uint<BITS>& b);
	friend bool operator!= <>(const base_uint<BITS>& a, uint64_t b);
	
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
	
    friend class uint160;
    friend class uint256;
    friend class uint512;
};

typedef base_uint<160> base_uint160;
typedef base_uint<256> base_uint256;
typedef base_uint<512> base_uint512;

#endif // BASE_UINT_H
