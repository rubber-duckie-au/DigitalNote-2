#include <cstddef>

#include "smsg/securemessage.h"

namespace DigitalNote {
namespace SMSG {

SecureMessage::SecureMessage()
{
	nPayload = 0;
	pPayload = NULL;
}

SecureMessage::~SecureMessage()
{
	if (pPayload)
	{
		delete[] pPayload;
	}
	pPayload = NULL;
}

} // namespace SMSG
} // namespace DigitalNote
