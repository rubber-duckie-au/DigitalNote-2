#ifndef BASE_UINT_H
#define BASE_UINT_H

#include <ostream>

/** Base class without constructors for uint256 and uint160.
 * This makes the compiler let u use it in a union.
 */
template<unsigned int BITS>
class base_uint
{
protected:
    enum {
		WIDTH=BITS/32
	};
	
    unsigned int pn[WIDTH];

public:
	friend class uint160;
    friend class uint256;
    friend class uint512;
	
    bool operator!() const;
    const base_uint<BITS> operator~() const;
    const base_uint<BITS> operator-() const;
    double getdouble() const;
	
    base_uint<BITS>& operator=(uint64_t b);
    base_uint<BITS>& operator^=(const base_uint<BITS>& b);
    base_uint<BITS>& operator&=(const base_uint<BITS>& b);
    base_uint<BITS>& operator|=(const base_uint<BITS>& b);
    base_uint<BITS>& operator^=(uint64_t b);
    base_uint<BITS>& operator|=(uint64_t b);
    base_uint<BITS>& operator<<=(unsigned int shift);
    base_uint<BITS>& operator>>=(unsigned int shift);
    base_uint<BITS>& operator+=(const base_uint<BITS>& b);
    base_uint<BITS>& operator-=(const base_uint<BITS>& b);
    base_uint<BITS>& operator+=(uint64_t b64);
    base_uint<BITS>& operator-=(uint64_t b64);
    base_uint<BITS>& operator++();
    const base_uint<BITS> operator++(int);
    base_uint<BITS>& operator--();
    const base_uint<BITS> operator--(int);
    
	template<unsigned int _BITS_>
	friend bool operator<(const base_uint<_BITS_>& a, const base_uint<_BITS_>& b);
    template<unsigned int _BITS_>
	friend bool operator<=(const base_uint<_BITS_>& a, const base_uint<_BITS_>& b);
    template<unsigned int _BITS_>
	friend bool operator>(const base_uint<_BITS_>& a, const base_uint<_BITS_>& b);
    template<unsigned int _BITS_>
	friend bool operator>=(const base_uint<_BITS_>& a, const base_uint<_BITS_>& b);
    template<unsigned int _BITS_>
	friend bool operator==(const base_uint<_BITS_>& a, const base_uint<_BITS_>& b);
    template<unsigned int _BITS_>
	friend bool operator==(const base_uint<_BITS_>& a, uint64_t b);
    template<unsigned int _BITS_>
	friend bool operator!=(const base_uint<_BITS_>& a, const base_uint<_BITS_>& b);
    template<unsigned int _BITS_>
	friend bool operator!=(const base_uint<_BITS_>& a, uint64_t b);
    
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
};

typedef base_uint<160> base_uint160;
typedef base_uint<256> base_uint256;
typedef base_uint<512> base_uint512;

#endif // BASE_UINT_H
