#ifndef CTXDESTINATION_H
#define CTXDESTINATION_H

#include <boost/variant.hpp>

class CNoDestination;
class CKeyID;
class CScriptID;
class CStealthAddress;

/** A txout script template with a specific destination. It is either:
 *  * CNoDestination: no destination set
 *  * CKeyID: TX_PUBKEYHASH destination
 *  * CScriptID: TX_SCRIPTHASH destination
 *  A CTxDestination is the internal data type encoded in a CDigitalNoteAddress
 */
using CTxDestination = boost::variant<CNoDestination, CKeyID, CScriptID, CStealthAddress>;

#endif // CTXDESTINATION_H
