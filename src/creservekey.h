#ifndef CRESERVEKEY_H
#define CRESERVEKEY_H

#include "pubkey.h"

class CWallet;

/** A key allocated from the key pool. */
class CReserveKey
{
protected:
    CWallet* pwallet;
    int64_t nIndex;
    CPubKey vchPubKey;

public:
    CReserveKey(CWallet* pwalletIn);
    ~CReserveKey();
	
    void ReturnKey();
    bool GetReservedKey(CPubKey &pubkey);
    void KeepKey();
};

#endif // CRESERVEKEY_H
