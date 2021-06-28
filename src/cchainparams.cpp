#include "cdnsseeddata.h"

#include "cchainparams.h"

CChainParams::CChainParams()
{
	
}

const uint256& CChainParams::HashGenesisBlock() const
{
	return hashGenesisBlock;
}

const MessageStartChars& CChainParams::MessageStart() const
{
	return pchMessageStart;
}

const std::vector<unsigned char>& CChainParams::AlertKey() const
{
	return vAlertPubKey;
}

int CChainParams::GetDefaultPort() const
{
	return nDefaultPort;
}

const CBigNum& CChainParams::ProofOfWorkLimit() const
{
	return bnProofOfWorkLimit;
}

const CBigNum& CChainParams::ProofOfStakeLimit() const
{
	return bnProofOfStakeLimit;
}

bool CChainParams::RequireRPCPassword() const
{
	return true;
}

const std::string& CChainParams::DataDir() const
{
	return strDataDir;
}

const std::vector<CDNSSeedData>& CChainParams::DNSSeeds() const
{
	return vSeeds;
}

const std::vector<unsigned char>& CChainParams::Base58Prefix(CChainParams_Base58Type type) const
{
	return base58Prefixes[type];
}

int CChainParams::RPCPort() const
{
	return nRPCPort;
}

int CChainParams::EndPoWBlock() const
{
	return nEndPoWBlock;
}

int CChainParams::StartPoSBlock() const
{
	return nStartPoSBlock;
}

int CChainParams::PoolMaxTransactions() const
{
	return nPoolMaxTransactions;
}

std::string CChainParams::MNenginePoolDummyAddress() const 
{
	return strMNenginePoolDummyAddress;
}

std::string CChainParams::DevOpsAddress() const
{
	return strDevOpsAddress;
}

