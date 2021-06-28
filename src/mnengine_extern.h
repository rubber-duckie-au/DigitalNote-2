#ifndef MNENGINE_EXTERN_H
#define MNENGINE_EXTERN_H

#include <string>
#include <vector>
#include <map>

class CMNenginePool;
class CMNengineSigner;
class CMNengineQueue;
class uint256;
class CMNengineBroadcastTx;
class CActiveMasternode;

extern CMNenginePool mnEnginePool;
extern CMNengineSigner mnEngineSigner;
extern std::vector<CMNengineQueue> vecMNengineQueue;
extern std::string strMasterNodePrivKey;
extern std::map<uint256, CMNengineBroadcastTx> mapMNengineBroadcastTxes;
extern CActiveMasternode activeMasternode;

#endif // MNENGINE_EXTERN_H
