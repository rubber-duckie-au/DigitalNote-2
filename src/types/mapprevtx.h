#ifndef MAPPREVTX_H
#define MAPPREVTX_H

#include <map>

class uint256;
class CTxIndex;
class CTransaction;

typedef std::map<uint256, std::pair<CTxIndex, CTransaction>> MapPrevTx;

#endif // MAPPREVTX_H