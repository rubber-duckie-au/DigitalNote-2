#include "uint/uint256.h"
#include "util.h"
#include "hash.h"

#include "csporkmessage.h"

uint256 CSporkMessage::GetHash()
{
	uint256 n = Hash(BEGIN(nSporkID), END(nTimeSigned));
	return n;
}

