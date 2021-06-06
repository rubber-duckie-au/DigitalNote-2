#include "cbignum_error.h"

#include "cbignum_ctx.h"

BN_CTX* CBigNum_CTX::operator=(BN_CTX* pnew)
{
	return pctx = pnew;
}

CBigNum_CTX::CBigNum_CTX()
{
	pctx = BN_CTX_new();
	
	if (pctx == NULL)
	{
		throw CBigNum_Error("CBigNum_CTX : BN_CTX_new() returned NULL");
	}
}

CBigNum_CTX::~CBigNum_CTX()
{
	if (pctx != NULL)
	{
		BN_CTX_free(pctx);
	}
}

CBigNum_CTX::operator BN_CTX*()
{
	return pctx;
}

BN_CTX& CBigNum_CTX::operator*()
{
	return *pctx;
}

BN_CTX** CBigNum_CTX::operator&()
{
	return &pctx;
}

bool CBigNum_CTX::operator!()
{
	return (pctx == NULL);
}

