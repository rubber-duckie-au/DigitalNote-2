#ifndef CTXDESTINATION_H
#define CTXDESTINATION_H

#include <boost/variant.hpp>

#include "cnodestination.h"
#include "ckeyid.h"
#include "cscriptid.h"
#include "cstealthaddress.h"

/** A txout script template with a specific destination. It is either:
 *  * CNoDestination: no destination set
 *  * CKeyID: TX_PUBKEYHASH destination
 *  * CScriptID: TX_SCRIPTHASH destination
 *  A CTxDestination is the internal data type encoded in a CDigitalNoteAddress
 */
typedef boost::variant<CNoDestination, CKeyID, CScriptID, CStealthAddress> CTxDestination;

#endif // CTXDESTINATION_H
