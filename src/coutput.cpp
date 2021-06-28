#include "compat.h"

#include "cwallettx.h"
#include "main_const.h"
#include "util.h"
#include "ctxout.h"

#include "coutput.h"

COutput::COutput(const CWalletTx *txIn, int iIn, int nDepthIn, bool fSpendableIn)
{
	tx = txIn;
	i = iIn;
	nDepth = nDepthIn;
	fSpendable = fSpendableIn;
}

std::string COutput::ToString() const
{
	return strprintf("COutput(%s, %d, %d) [%s]", tx->GetHash().ToString(), i, nDepth, FormatMoney(tx->vout[i].nValue));
}

//Used with MNengine. Will return fees, then everything else, then very small inputs that aren't fees
int COutput::Priority() const
{
	//nondenom return largest first
	return -(tx->vout[i].nValue / COIN);
}

void COutput::print() const
{
	LogPrintf("%s\n", ToString().c_str());
}

