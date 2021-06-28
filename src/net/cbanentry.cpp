#include "net/banreason.h"
#include "serialize.h"
#include "cdatastream.h"

#include "net/cbanentry.h"

CBanEntry::CBanEntry()
{
	SetNull();
}

CBanEntry::CBanEntry(int64_t nCreateTimeIn)
{
	SetNull();
	nCreateTime = nCreateTimeIn;
}

unsigned int CBanEntry::GetSerializeSize(int nType, int nVersion) const
{
	CSerActionGetSerializeSize ser_action;
	const bool fGetSize = true;
	const bool fWrite = false;
	const bool fRead = false;
	unsigned int nSerSize = 0;
	ser_streamplaceholder s;
	assert(fGetSize||fWrite||fRead); /* suppress warning */
	s.nType = nType;
	s.nVersion = nVersion;
	
	READWRITE(this->nVersion);
	nVersion = this->nVersion;
	READWRITE(nCreateTime);
	READWRITE(nBanUntil);
	READWRITE(banReason);
	
	return nSerSize;
}

template<typename Stream>
void CBanEntry::Serialize(Stream& s, int nType, int nVersion) const
{
	CSerActionSerialize ser_action;
	const bool fGetSize = false;
	const bool fWrite = true;
	const bool fRead = false;
	unsigned int nSerSize = 0;
	assert(fGetSize||fWrite||fRead); /* suppress warning */
	
	READWRITE(this->nVersion);
	nVersion = this->nVersion;
	READWRITE(nCreateTime);
	READWRITE(nBanUntil);
	READWRITE(banReason);
}

template<typename Stream>
void CBanEntry::Unserialize(Stream& s, int nType, int nVersion)
{
	CSerActionUnserialize ser_action;
	const bool fGetSize = false;
	const bool fWrite = false;
	const bool fRead = true;
	unsigned int nSerSize = 0;
	assert(fGetSize||fWrite||fRead); /* suppress warning */
	
	READWRITE(this->nVersion);
	nVersion = this->nVersion;
	READWRITE(nCreateTime);
	READWRITE(nBanUntil);
	READWRITE(banReason);
}

template void CBanEntry::Serialize<CDataStream>(CDataStream& s, int nType, int nVersion) const;
template void CBanEntry::Unserialize<CDataStream>(CDataStream& s, int nType, int nVersion);

void CBanEntry::SetNull()
{
	nVersion = CBanEntry::CURRENT_VERSION;
	nCreateTime = 0;
	nBanUntil = 0;
	banReason = BanReasonUnknown;
}

std::string CBanEntry::banReasonToString()
{
	switch (banReason) {
	case BanReasonNodeMisbehaving:
		return "node misbehaving";
	case BanReasonManuallyAdded:
		return "manually added";
	default:
		return "unknown";
	}
}

