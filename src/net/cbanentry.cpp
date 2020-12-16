#include "net/banreason.h"

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

