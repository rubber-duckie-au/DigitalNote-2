#ifndef CACCOUNT_H
#define CACCOUNT_H

#include "pubkey.h"
#include "serialize.h"

/** Account information.
 * Stored in wallet with key "acc"+string account name.
 */
class CAccount
{
public:
    CPubKey vchPubKey;

    CAccount();
    void SetNull();

    IMPLEMENT_SERIALIZE
    (
        if (!(nType & SER_GETHASH))
            READWRITE(nVersion);
        READWRITE(vchPubKey);
    )
};

#endif // CACCOUNT_H
