#ifndef CEXTKEY_H
#define CEXTKEY_H

#include "ckey.h"

struct CExtPubKey;

struct CExtKey
{
    unsigned char nDepth;
    unsigned char vchFingerprint[4];
    unsigned int nChild;
    unsigned char vchChainCode[32];
    CKey key;

    friend bool operator==(const CExtKey &a, const CExtKey &b)
	{
        return (
			a.nDepth == b.nDepth &&
			memcmp(&a.vchFingerprint[0], &b.vchFingerprint[0], 4) == 0 &&
			a.nChild == b.nChild &&
            memcmp(&a.vchChainCode[0], &b.vchChainCode[0], 32) == 0 &&
			a.key == b.key
		);
    }

    void Encode(unsigned char code[74]) const;
    void Decode(const unsigned char code[74]);
    bool Derive(CExtKey &out, unsigned int nChild) const;
    CExtPubKey Neuter() const;
    void SetMaster(const unsigned char *seed, unsigned int nSeedLen);
};

#endif // CEXTKEY_H
