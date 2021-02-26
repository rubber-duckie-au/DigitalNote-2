#ifndef CREGTESTPARAMS_H
#define CREGTESTPARAMS_H

#include "ctestnetparams.h"

//
// Regression test
//
class CRegTestParams : public CTestNetParams
{
public:
    CRegTestParams();
	
    virtual bool RequireRPCPassword() const;
    virtual CChainParams::Network NetworkID() const;
};

#endif // CREGTESTPARAMS_H
