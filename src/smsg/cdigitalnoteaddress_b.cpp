#include "smsg/cdigitalnoteaddress_b.h"

namespace DigitalNote {
namespace SMSG {

uint8_t CDigitalNoteAddress_B::getVersion()
{
	// TODO: fix
	if (vchVersion.size() > 0)
	{
		return vchVersion[0];
	}
	
	return 0;
}

} // namespace SMSG
} // namespace DigitalNote
