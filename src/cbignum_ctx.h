#ifndef CBIGNUM_CTX_H
#define CBIGNUM_CTX_H

#include <openssl/bn.h>

/** RAII encapsulated BN_CTX (OpenSSL bignum context) */
class CBigNum_CTX
{
protected:
	BN_CTX* pctx;
	BN_CTX* operator=(BN_CTX* pnew);

public:
	CBigNum_CTX();
	~CBigNum_CTX();
	
	operator BN_CTX*();
	BN_CTX& operator*();
	BN_CTX** operator&();
	bool operator!();
};

#endif // CBIGNUM_CTX_H
