#ifndef CBANENTRY_H
#define CBANENTRY_H

#include <string>

#include "serialize.h"

class CBanEntry
{
public:
    static const int CURRENT_VERSION=1;
    int nVersion;
    int64_t nCreateTime;
    int64_t nBanUntil;
    uint8_t banReason;

    CBanEntry();
    CBanEntry(int64_t nCreateTimeIn);
  
    IMPLEMENT_SERIALIZE
    (
        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(nCreateTime);
        READWRITE(nBanUntil);
        READWRITE(banReason);
    )

    void SetNull();
    std::string banReasonToString();
};

#endif // CBANENTRY_H
