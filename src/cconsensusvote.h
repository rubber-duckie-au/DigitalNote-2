#ifndef CCONSENSUSVOTE_H
#define CCONSENSUSVOTE_H

#include <vector>

#include "serialize.h"
#include "uint/uint256.h"
#include "chain.h"

class CConsensusVote
{
public:
    CTxIn vinMasternode;
    uint256 txHash;
    int nBlockHeight;
    std::vector<unsigned char> vchMasterNodeSignature;

    IMPLEMENT_SERIALIZE
    (
        READWRITE(txHash);
        READWRITE(vinMasternode);
        READWRITE(vchMasterNodeSignature);
        READWRITE(nBlockHeight);
    )
	
    uint256 GetHash() const;
    bool SignatureValid();
    bool Sign();
};

#endif // CCONSENSUSVOTE_H
