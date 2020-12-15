// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_UINT256_H
#define BITCOIN_UINT256_H

#include <string>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//
// uint160 and uint256 could be implemented as templates, but to keep
// compile errors and debugging cleaner, they're copy and pasted.
//

#include "base_uint.h"

//////////////////////////////////////////////////////////////////////////////
//
// uint160
//

/** 160-bit unsigned integer */
class uint160 : public base_uint160
{
public:
    typedef base_uint160 basetype;

    uint160()
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = 0;
    }

    uint160(const basetype& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = b.pn[i];
    }

    uint160& operator=(const basetype& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = b.pn[i];
        return *this;
    }

    uint160(uint64_t b)
    {
        pn[0] = (unsigned int)b;
        pn[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            pn[i] = 0;
    }

    uint160& operator=(uint64_t b)
    {
        pn[0] = (unsigned int)b;
        pn[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            pn[i] = 0;
        return *this;
    }

    explicit uint160(const std::string& str)
    {
        SetHex(str);
    }

    explicit uint160(const std::vector<unsigned char>& vch)
    {
        if (vch.size() == sizeof(pn))
            memcpy(pn, &vch[0], sizeof(pn));
        else
            *this = 0;
    }
};

inline bool operator==(const uint160& a, uint64_t b)                         { return (base_uint160)a == b; }
inline bool operator!=(const uint160& a, uint64_t b)                         { return (base_uint160)a != b; }
inline const uint160 operator<<(const base_uint160& a, unsigned int shift)   { return uint160(a) <<= shift; }
inline const uint160 operator>>(const base_uint160& a, unsigned int shift)   { return uint160(a) >>= shift; }
inline const uint160 operator<<(const uint160& a, unsigned int shift)        { return uint160(a) <<= shift; }
inline const uint160 operator>>(const uint160& a, unsigned int shift)        { return uint160(a) >>= shift; }

inline const uint160 operator^(const base_uint160& a, const base_uint160& b) { return uint160(a) ^= b; }
inline const uint160 operator&(const base_uint160& a, const base_uint160& b) { return uint160(a) &= b; }
inline const uint160 operator|(const base_uint160& a, const base_uint160& b) { return uint160(a) |= b; }
inline const uint160 operator+(const base_uint160& a, const base_uint160& b) { return uint160(a) += b; }
inline const uint160 operator-(const base_uint160& a, const base_uint160& b) { return uint160(a) -= b; }

inline bool operator<(const base_uint160& a, const uint160& b)          { return (base_uint160)a <  (base_uint160)b; }
inline bool operator<=(const base_uint160& a, const uint160& b)         { return (base_uint160)a <= (base_uint160)b; }
inline bool operator>(const base_uint160& a, const uint160& b)          { return (base_uint160)a >  (base_uint160)b; }
inline bool operator>=(const base_uint160& a, const uint160& b)         { return (base_uint160)a >= (base_uint160)b; }
inline bool operator==(const base_uint160& a, const uint160& b)         { return (base_uint160)a == (base_uint160)b; }
inline bool operator!=(const base_uint160& a, const uint160& b)         { return (base_uint160)a != (base_uint160)b; }
inline const uint160 operator^(const base_uint160& a, const uint160& b) { return (base_uint160)a ^  (base_uint160)b; }
inline const uint160 operator&(const base_uint160& a, const uint160& b) { return (base_uint160)a &  (base_uint160)b; }
inline const uint160 operator|(const base_uint160& a, const uint160& b) { return (base_uint160)a |  (base_uint160)b; }
inline const uint160 operator+(const base_uint160& a, const uint160& b) { return (base_uint160)a +  (base_uint160)b; }
inline const uint160 operator-(const base_uint160& a, const uint160& b) { return (base_uint160)a -  (base_uint160)b; }

inline bool operator<(const uint160& a, const base_uint160& b)          { return (base_uint160)a <  (base_uint160)b; }
inline bool operator<=(const uint160& a, const base_uint160& b)         { return (base_uint160)a <= (base_uint160)b; }
inline bool operator>(const uint160& a, const base_uint160& b)          { return (base_uint160)a >  (base_uint160)b; }
inline bool operator>=(const uint160& a, const base_uint160& b)         { return (base_uint160)a >= (base_uint160)b; }
inline bool operator==(const uint160& a, const base_uint160& b)         { return (base_uint160)a == (base_uint160)b; }
inline bool operator!=(const uint160& a, const base_uint160& b)         { return (base_uint160)a != (base_uint160)b; }
inline const uint160 operator^(const uint160& a, const base_uint160& b) { return (base_uint160)a ^  (base_uint160)b; }
inline const uint160 operator&(const uint160& a, const base_uint160& b) { return (base_uint160)a &  (base_uint160)b; }
inline const uint160 operator|(const uint160& a, const base_uint160& b) { return (base_uint160)a |  (base_uint160)b; }
inline const uint160 operator+(const uint160& a, const base_uint160& b) { return (base_uint160)a +  (base_uint160)b; }
inline const uint160 operator-(const uint160& a, const base_uint160& b) { return (base_uint160)a -  (base_uint160)b; }

inline bool operator<(const uint160& a, const uint160& b)               { return (base_uint160)a <  (base_uint160)b; }
inline bool operator<=(const uint160& a, const uint160& b)              { return (base_uint160)a <= (base_uint160)b; }
inline bool operator>(const uint160& a, const uint160& b)               { return (base_uint160)a >  (base_uint160)b; }
inline bool operator>=(const uint160& a, const uint160& b)              { return (base_uint160)a >= (base_uint160)b; }
inline bool operator==(const uint160& a, const uint160& b)              { return (base_uint160)a == (base_uint160)b; }
inline bool operator!=(const uint160& a, const uint160& b)              { return (base_uint160)a != (base_uint160)b; }
inline const uint160 operator^(const uint160& a, const uint160& b)      { return (base_uint160)a ^  (base_uint160)b; }
inline const uint160 operator&(const uint160& a, const uint160& b)      { return (base_uint160)a &  (base_uint160)b; }
inline const uint160 operator|(const uint160& a, const uint160& b)      { return (base_uint160)a |  (base_uint160)b; }
inline const uint160 operator+(const uint160& a, const uint160& b)      { return (base_uint160)a +  (base_uint160)b; }
inline const uint160 operator-(const uint160& a, const uint160& b)      { return (base_uint160)a -  (base_uint160)b; }






//////////////////////////////////////////////////////////////////////////////
//
// uint256
//

/** 256-bit unsigned integer */
class uint256 : public base_uint256
{
public:
    typedef base_uint256 basetype;

    uint256()
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = 0;
    }

    uint256(const basetype& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = b.pn[i];
    }

    uint256& operator=(const basetype& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = b.pn[i];
        return *this;
    }

    uint256(uint64_t b)
    {
        pn[0] = (unsigned int)b;
        pn[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            pn[i] = 0;
    }

    uint256& operator=(uint64_t b)
    {
        pn[0] = (unsigned int)b;
        pn[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            pn[i] = 0;
        return *this;
    }

    explicit uint256(const std::string& str)
    {
        SetHex(str);
    }

    explicit uint256(const std::vector<unsigned char>& vch)
    {
        if (vch.size() == sizeof(pn))
            memcpy(pn, &vch[0], sizeof(pn));
        else
            *this = 0;
    }
};

inline bool operator==(const uint256& a, uint64_t b)                         { return (base_uint256)a == b; }
inline bool operator!=(const uint256& a, uint64_t b)                         { return (base_uint256)a != b; }
inline const uint256 operator<<(const base_uint256& a, unsigned int shift)   { return uint256(a) <<= shift; }
inline const uint256 operator>>(const base_uint256& a, unsigned int shift)   { return uint256(a) >>= shift; }
inline const uint256 operator<<(const uint256& a, unsigned int shift)        { return uint256(a) <<= shift; }
inline const uint256 operator>>(const uint256& a, unsigned int shift)        { return uint256(a) >>= shift; }

inline const uint256 operator^(const base_uint256& a, const base_uint256& b) { return uint256(a) ^= b; }
inline const uint256 operator&(const base_uint256& a, const base_uint256& b) { return uint256(a) &= b; }
inline const uint256 operator|(const base_uint256& a, const base_uint256& b) { return uint256(a) |= b; }
inline const uint256 operator+(const base_uint256& a, const base_uint256& b) { return uint256(a) += b; }
inline const uint256 operator-(const base_uint256& a, const base_uint256& b) { return uint256(a) -= b; }

inline bool operator<(const base_uint256& a, const uint256& b)          { return (base_uint256)a <  (base_uint256)b; }
inline bool operator<=(const base_uint256& a, const uint256& b)         { return (base_uint256)a <= (base_uint256)b; }
inline bool operator>(const base_uint256& a, const uint256& b)          { return (base_uint256)a >  (base_uint256)b; }
inline bool operator>=(const base_uint256& a, const uint256& b)         { return (base_uint256)a >= (base_uint256)b; }
inline bool operator==(const base_uint256& a, const uint256& b)         { return (base_uint256)a == (base_uint256)b; }
inline bool operator!=(const base_uint256& a, const uint256& b)         { return (base_uint256)a != (base_uint256)b; }
inline const uint256 operator^(const base_uint256& a, const uint256& b) { return (base_uint256)a ^  (base_uint256)b; }
inline const uint256 operator&(const base_uint256& a, const uint256& b) { return (base_uint256)a &  (base_uint256)b; }
inline const uint256 operator|(const base_uint256& a, const uint256& b) { return (base_uint256)a |  (base_uint256)b; }
inline const uint256 operator+(const base_uint256& a, const uint256& b) { return (base_uint256)a +  (base_uint256)b; }
inline const uint256 operator-(const base_uint256& a, const uint256& b) { return (base_uint256)a -  (base_uint256)b; }

inline bool operator<(const uint256& a, const base_uint256& b)          { return (base_uint256)a <  (base_uint256)b; }
inline bool operator<=(const uint256& a, const base_uint256& b)         { return (base_uint256)a <= (base_uint256)b; }
inline bool operator>(const uint256& a, const base_uint256& b)          { return (base_uint256)a >  (base_uint256)b; }
inline bool operator>=(const uint256& a, const base_uint256& b)         { return (base_uint256)a >= (base_uint256)b; }
inline bool operator==(const uint256& a, const base_uint256& b)         { return (base_uint256)a == (base_uint256)b; }
inline bool operator!=(const uint256& a, const base_uint256& b)         { return (base_uint256)a != (base_uint256)b; }
inline const uint256 operator^(const uint256& a, const base_uint256& b) { return (base_uint256)a ^  (base_uint256)b; }
inline const uint256 operator&(const uint256& a, const base_uint256& b) { return (base_uint256)a &  (base_uint256)b; }
inline const uint256 operator|(const uint256& a, const base_uint256& b) { return (base_uint256)a |  (base_uint256)b; }
inline const uint256 operator+(const uint256& a, const base_uint256& b) { return (base_uint256)a +  (base_uint256)b; }
inline const uint256 operator-(const uint256& a, const base_uint256& b) { return (base_uint256)a -  (base_uint256)b; }

inline bool operator<(const uint256& a, const uint256& b)               { return (base_uint256)a <  (base_uint256)b; }
inline bool operator<=(const uint256& a, const uint256& b)              { return (base_uint256)a <= (base_uint256)b; }
inline bool operator>(const uint256& a, const uint256& b)               { return (base_uint256)a >  (base_uint256)b; }
inline bool operator>=(const uint256& a, const uint256& b)              { return (base_uint256)a >= (base_uint256)b; }
inline bool operator==(const uint256& a, const uint256& b)              { return (base_uint256)a == (base_uint256)b; }
inline bool operator!=(const uint256& a, const uint256& b)              { return (base_uint256)a != (base_uint256)b; }
inline const uint256 operator^(const uint256& a, const uint256& b)      { return (base_uint256)a ^  (base_uint256)b; }
inline const uint256 operator&(const uint256& a, const uint256& b)      { return (base_uint256)a &  (base_uint256)b; }
inline const uint256 operator|(const uint256& a, const uint256& b)      { return (base_uint256)a |  (base_uint256)b; }
inline const uint256 operator+(const uint256& a, const uint256& b)      { return (base_uint256)a +  (base_uint256)b; }
inline const uint256 operator-(const uint256& a, const uint256& b)      { return (base_uint256)a -  (base_uint256)b; }







//////////////////////////////////////////////////////////////////////////////
//
// uint512
//

/** 512-bit unsigned integer */
class uint512 : public base_uint512
{
public:
    typedef base_uint512 basetype;

    uint512()
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = 0;
    }

    uint512(const basetype& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = b.pn[i];
    }

    uint512& operator=(const basetype& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = b.pn[i];
        return *this;
    }

    uint512(uint64_t b)
    {
        pn[0] = (unsigned int)b;
        pn[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            pn[i] = 0;
    }

    uint512& operator=(uint64_t b)
    {
        pn[0] = (unsigned int)b;
        pn[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            pn[i] = 0;
        return *this;
    }

    explicit uint512(const std::string& str)
    {
        SetHex(str);
    }

    explicit uint512(const std::vector<unsigned char>& vch)
    {
        if (vch.size() == sizeof(pn))
            memcpy(pn, &vch[0], sizeof(pn));
        else
            *this = 0;
    }

    uint256 trim256() const
    {
        uint256 ret;
        for (unsigned int i = 0; i < uint256::WIDTH; i++){
            ret.pn[i] = pn[i];
        }
        return ret;
    }
};

inline bool operator==(const uint512& a, uint64_t b)                           { return (base_uint512)a == b; }
inline bool operator!=(const uint512& a, uint64_t b)                           { return (base_uint512)a != b; }
inline const uint512 operator<<(const base_uint512& a, unsigned int shift)   { return uint512(a) <<= shift; }
inline const uint512 operator>>(const base_uint512& a, unsigned int shift)   { return uint512(a) >>= shift; }
inline const uint512 operator<<(const uint512& a, unsigned int shift)        { return uint512(a) <<= shift; }
inline const uint512 operator>>(const uint512& a, unsigned int shift)        { return uint512(a) >>= shift; }

inline const uint512 operator^(const base_uint512& a, const base_uint512& b) { return uint512(a) ^= b; }
inline const uint512 operator&(const base_uint512& a, const base_uint512& b) { return uint512(a) &= b; }
inline const uint512 operator|(const base_uint512& a, const base_uint512& b) { return uint512(a) |= b; }
inline const uint512 operator+(const base_uint512& a, const base_uint512& b) { return uint512(a) += b; }
inline const uint512 operator-(const base_uint512& a, const base_uint512& b) { return uint512(a) -= b; }

inline bool operator<(const base_uint512& a, const uint512& b)          { return (base_uint512)a <  (base_uint512)b; }
inline bool operator<=(const base_uint512& a, const uint512& b)         { return (base_uint512)a <= (base_uint512)b; }
inline bool operator>(const base_uint512& a, const uint512& b)          { return (base_uint512)a >  (base_uint512)b; }
inline bool operator>=(const base_uint512& a, const uint512& b)         { return (base_uint512)a >= (base_uint512)b; }
inline bool operator==(const base_uint512& a, const uint512& b)         { return (base_uint512)a == (base_uint512)b; }
inline bool operator!=(const base_uint512& a, const uint512& b)         { return (base_uint512)a != (base_uint512)b; }
inline const uint512 operator^(const base_uint512& a, const uint512& b) { return (base_uint512)a ^  (base_uint512)b; }
inline const uint512 operator&(const base_uint512& a, const uint512& b) { return (base_uint512)a &  (base_uint512)b; }
inline const uint512 operator|(const base_uint512& a, const uint512& b) { return (base_uint512)a |  (base_uint512)b; }
inline const uint512 operator+(const base_uint512& a, const uint512& b) { return (base_uint512)a +  (base_uint512)b; }
inline const uint512 operator-(const base_uint512& a, const uint512& b) { return (base_uint512)a -  (base_uint512)b; }

inline bool operator<(const uint512& a, const base_uint512& b)          { return (base_uint512)a <  (base_uint512)b; }
inline bool operator<=(const uint512& a, const base_uint512& b)         { return (base_uint512)a <= (base_uint512)b; }
inline bool operator>(const uint512& a, const base_uint512& b)          { return (base_uint512)a >  (base_uint512)b; }
inline bool operator>=(const uint512& a, const base_uint512& b)         { return (base_uint512)a >= (base_uint512)b; }
inline bool operator==(const uint512& a, const base_uint512& b)         { return (base_uint512)a == (base_uint512)b; }
inline bool operator!=(const uint512& a, const base_uint512& b)         { return (base_uint512)a != (base_uint512)b; }
inline const uint512 operator^(const uint512& a, const base_uint512& b) { return (base_uint512)a ^  (base_uint512)b; }
inline const uint512 operator&(const uint512& a, const base_uint512& b) { return (base_uint512)a &  (base_uint512)b; }
inline const uint512 operator|(const uint512& a, const base_uint512& b) { return (base_uint512)a |  (base_uint512)b; }
inline const uint512 operator+(const uint512& a, const base_uint512& b) { return (base_uint512)a +  (base_uint512)b; }
inline const uint512 operator-(const uint512& a, const base_uint512& b) { return (base_uint512)a -  (base_uint512)b; }

inline bool operator<(const uint512& a, const uint512& b)               { return (base_uint512)a <  (base_uint512)b; }
inline bool operator<=(const uint512& a, const uint512& b)              { return (base_uint512)a <= (base_uint512)b; }
inline bool operator>(const uint512& a, const uint512& b)               { return (base_uint512)a >  (base_uint512)b; }
inline bool operator>=(const uint512& a, const uint512& b)              { return (base_uint512)a >= (base_uint512)b; }
inline bool operator==(const uint512& a, const uint512& b)              { return (base_uint512)a == (base_uint512)b; }
inline bool operator!=(const uint512& a, const uint512& b)              { return (base_uint512)a != (base_uint512)b; }
inline const uint512 operator^(const uint512& a, const uint512& b)      { return (base_uint512)a ^  (base_uint512)b; }
inline const uint512 operator&(const uint512& a, const uint512& b)      { return (base_uint512)a &  (base_uint512)b; }
inline const uint512 operator|(const uint512& a, const uint512& b)      { return (base_uint512)a |  (base_uint512)b; }
inline const uint512 operator+(const uint512& a, const uint512& b)      { return (base_uint512)a +  (base_uint512)b; }
inline const uint512 operator-(const uint512& a, const uint512& b)      { return (base_uint512)a -  (base_uint512)b; }

#endif
