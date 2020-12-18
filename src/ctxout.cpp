#include "util.h"

#include "ctxout.h"

CTxOut::CTxOut()
{
	SetNull();
}

CTxOut::CTxOut(int64_t nValueIn, CScript scriptPubKeyIn)
{
    nValue = nValueIn;
    scriptPubKey = scriptPubKeyIn;
}

void CTxOut::SetNull()
{
	nValue = -1;
	nRounds = -10; // an initial value, should be no way to get this by calculations
	scriptPubKey.clear();
}

bool CTxOut::IsNull() const
{
	return (nValue == -1);
}

void CTxOut::SetEmpty()
{
	nValue = 0;
	scriptPubKey.clear();
}

bool CTxOut::IsEmpty() const
{
	return (nValue == 0 && scriptPubKey.empty());
}

uint256 CTxOut::GetHash() const
{
    return SerializeHash(*this);
}

bool CTxOut::IsDust(int64_t MIN_RELAY_TX_FEE) const
{
	// "Dust" is defined in terms of CTransaction::nMinRelayTxFee,
	// which has units satoshis-per-kilobyte.
	// If you'd pay more than 1/3 in fees
	// to spend something, then we consider it dust.
	// A typical txout is 34 bytes big, and will
	// need a CTxIn of at least 148 bytes to spend,
	// so dust is a txout less than 546 satoshis
	// with default nMinRelayTxFee.
	return ((nValue*1000)/(3*((int)GetSerializeSize(SER_DISK,0)+148)) < MIN_RELAY_TX_FEE);
}

bool operator==(const CTxOut& a, const CTxOut& b)
{
	return (a.nValue       == b.nValue &&
			a.nRounds      == b.nRounds &&
			a.scriptPubKey == b.scriptPubKey);
}

bool operator!=(const CTxOut& a, const CTxOut& b)
{
	return !(a == b);
}

std::string CTxOut::ToString() const
{
    if (IsEmpty()) return "CTxOut(empty)";
    return strprintf("CTxOut(nValue=%s, scriptPubKey=%s)", FormatMoney(nValue), scriptPubKey.ToString());
}

