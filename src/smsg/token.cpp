#include <cstring>

#include "smsg/token.h"

namespace DigitalNote {
namespace SMSG {

Token::Token()
{
	
}

Token::Token(int64_t ts, uint8_t* p, int np, long int o)
{
	timestamp = ts;

	if (np < 8) // payload will always be > 8, just make sure
	{
		memset(sample, 0, 8);
	}
	else
	{
		memcpy(sample, p, 8);
	}
	
	offset = o;
}

Token::~Token()
{
	
}

bool Token::operator <(const Token& y) const
{
	// pack and memcmp from timesent?
	if (timestamp == y.timestamp)
	{
		return memcmp(sample, y.sample, 8) < 0;
	}
	
	return timestamp < y.timestamp;
}

} // namespace SMSG
} // namespace DigitalNote
