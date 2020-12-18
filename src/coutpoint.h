#ifndef COUTPOINT_H
#define COUTPOINT_H

#include <string>

#include "serialize.h"
#include "uint/uint256.h"

class COutPoint;

bool operator<(const COutPoint& a, const COutPoint& b);
bool operator==(const COutPoint& a, const COutPoint& b);
bool operator!=(const COutPoint& a, const COutPoint& b);

/** An outpoint - a combination of a transaction hash and an index n into its vout */
class COutPoint
{
public:
    uint256 hash;
    unsigned int n;

    COutPoint();
    COutPoint(uint256 hashIn, unsigned int nIn);
	
    IMPLEMENT_SERIALIZE(
		READWRITE(FLATDATA(*this));
	)
	
    void SetNull();
    bool IsNull() const;
    
	friend bool operator<(const COutPoint& a, const COutPoint& b);
    friend bool operator==(const COutPoint& a, const COutPoint& b);
    friend bool operator!=(const COutPoint& a, const COutPoint& b);

    std::string ToString() const;
    std::string ToStringShort() const;
};

#endif // COUTPOINT_H
