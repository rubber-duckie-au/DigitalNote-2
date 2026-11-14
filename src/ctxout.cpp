#include "compat.h"

#include "util.h"
#include "serialize.h"
#include "chashwriter.h"
#include "uint/uint256.h"
#include "hash.h"
#include "cautofile.h"
#include "cdatastream.h"

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

unsigned int CTxOut::GetSerializeSize(int nType, int nVersion) const
{
	CSerActionGetSerializeSize ser_action;
	unsigned int nSerSize = 0;
	ser_streamplaceholder s;

	s.nType = nType;
	s.nVersion = nVersion;

	READWRITE(nValue);
	READWRITE(scriptPubKey);

	return nSerSize;
}

template<typename Stream>
void CTxOut::Serialize(Stream& s, int nType, int nVersion) const
{
	CSerActionSerialize ser_action;
	unsigned int nSerSize = 0;

	READWRITE(nValue);
	READWRITE(scriptPubKey);
}

template<typename Stream>
void CTxOut::Unserialize(Stream& s, int nType, int nVersion)
{
	CSerActionUnserialize ser_action;
	unsigned int nSerSize = 0;

	READWRITE(nValue);
	READWRITE(scriptPubKey);
}

template void CTxOut::Serialize<CDataStream>(CDataStream& s, int nType, int nVersion) const;
template void CTxOut::Unserialize<CDataStream>(CDataStream& s, int nType, int nVersion);
template void CTxOut::Serialize<CAutoFile>(CAutoFile& s, int nType, int nVersion) const;
template void CTxOut::Unserialize<CAutoFile>(CAutoFile& s, int nType, int nVersion);
template void CTxOut::Serialize<CHashWriter>(CHashWriter& s, int nType, int nVersion) const;

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
	//
	// >>> v2.0.0.9 FINDING-2026-016: DO NOT restore the original form. <<<
	//
	// This was:
	//     return ((nValue * 1000) / (3 * (GetSerializeSize(...) + 148)) < MIN_RELAY_TX_FEE);
	//
	// which evaluated (nValue * 1000) FIRST and overflowed signed 64-bit above
	//     (2^63 - 1) / 1000 = 9,223,372,036,854,775 units = 92,233,720 XDN.
	// The product wrapped NEGATIVE, the comparison was trivially true, every
	// such output was declared dust, and CreateTransaction failed with
	// "Transaction amount too small" -- for the LARGEST sends the wallet can be
	// asked to make. Reported on rc3, 2026-11-05.
	//
	// Upstream Bitcoin cannot reach this: its entire supply is 21M at 1e8 units,
	// so no output comes close. XDN's supply is far larger -- a single testnet
	// reserve block pays 80,000,000 XDN -- so consolidating two exceeds it. An
	// inherited assumption that stopped holding when the money supply changed.
	//
	// Rearranged so only SMALL values are multiplied. Multiplying both sides of
	// the original by nSpendCost/1000 gives exactly this; the two forms agree for
	// every value that did not overflow. MIN_RELAY_TX_FEE * nSpendCost is at most
	// a few million, nowhere near the limit.
	const int64_t nSpendCost = 3 * ((int)GetSerializeSize(SER_DISK, 0) + 148);

	return nValue < (MIN_RELAY_TX_FEE * nSpendCost) / 1000;
}

bool operator==(const CTxOut& a, const CTxOut& b)
{
	return (a.nValue == b.nValue &&
			a.nRounds == b.nRounds &&
			a.scriptPubKey == b.scriptPubKey);
}

bool operator!=(const CTxOut& a, const CTxOut& b)
{
	return !(a == b);
}

std::string CTxOut::ToString() const
{
	if (IsEmpty())
	{
		return "CTxOut(empty)";
	}
	
	return strprintf("CTxOut(nValue=%s, scriptPubKey=%s)", FormatMoney(nValue), scriptPubKey.ToString());
}

