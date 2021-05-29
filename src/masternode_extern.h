#ifndef MASTERNODE_EXTERN_H
#define MASTERNODE_EXTERN_H

#include <map>

#include "ccriticalsection.h"

class uint256;
class CMasternodeMan;

extern CCriticalSection cs_masternodes;
extern std::map<int64_t, uint256> mapCacheBlockHashes;
extern CMasternodeMan mnodeman;

#endif // MASTERNODE_EXTERN_H