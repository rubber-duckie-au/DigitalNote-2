#include <map>

#include "uint/uint256.h"
#include "cblockindex.h"
#include "chainparams.h"
#include "cchainparams.h"
#include "serialize.h"
#include "enums/serialize_type.h"
#include "main_extern.h"
#include "cdatastream.h"

#include "cblocklocator.h"

// v2.0.0.9 rc11: the most entries Set() puts in a locator before the genesis
// hash.  Set() steps back one block at a time, so building a locator all the
// way to genesis touched every block index entry (~1.25 million on mainnet)
// while cs_main was held.  On a host short of memory, where the index had been
// paged out, that stalled a node for 15 s or more every 144 blocks, when
// SetBestChain saves the wallet's locator (fleet.md, 2026-11-23/24).
//
// 25 entries sit at depths 0-10, 11, 13, 17, 25, 41, 73, 137, 265, 521, 1033,
// 2057, 4105, 8201 and 16393: a walk of ~16,400 blocks instead of the whole
// chain, and constant as the chain grows.  The list is a prefix of the
// unbounded one, so a peer finds the same common block for any fork up to
// that depth.  Only a fork deeper than ~16,400 blocks now matches only at
// genesis, and a node cannot rejoin such a fork through sync anyway:
// Checkpoints::CheckSync refuses any block 5,000 or more below its own tip.
static const unsigned int LOCATOR_MAX_ENTRIES = 25;

CBlockLocator::CBlockLocator()
{
	
}

CBlockLocator::CBlockLocator(const CBlockIndex* pindex)
{
	Set(pindex);
}

CBlockLocator::CBlockLocator(uint256 hashBlock)
{
	std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hashBlock);
	
	if (mi != mapBlockIndex.end())
	{
		Set((*mi).second);
	}
}

CBlockLocator::CBlockLocator(const std::vector<uint256>& vHaveIn)
{
	vHave = vHaveIn;
}

unsigned int CBlockLocator::GetSerializeSize(int nType, int nVersion) const
{
	CSerActionGetSerializeSize ser_action;
	unsigned int nSerSize = 0;
	ser_streamplaceholder s;

	s.nType = nType;
	s.nVersion = nVersion;
	
	if (!(nType & SER_GETHASH))
	{
		READWRITE(nVersion);
	}
	
	READWRITE(vHave);
	
	return nSerSize;
}

template<typename Stream>
void CBlockLocator::Serialize(Stream& s, int nType, int nVersion) const
{
	CSerActionSerialize ser_action;
	unsigned int nSerSize = 0;
	
	if (!(nType & SER_GETHASH))
	{
		READWRITE(nVersion);
	}
	
	READWRITE(vHave);
}

template<typename Stream>
void CBlockLocator::Unserialize(Stream& s, int nType, int nVersion)
{
	CSerActionUnserialize ser_action;
	unsigned int nSerSize = 0;
	
	if (!(nType & SER_GETHASH))
	{
		READWRITE(nVersion);
	}
	
	READWRITE(vHave);
}

template void CBlockLocator::Serialize<CDataStream>(CDataStream& s, int nType, int nVersion) const;
template void CBlockLocator::Unserialize<CDataStream>(CDataStream& s, int nType, int nVersion);

void CBlockLocator::SetNull()
{
	vHave.clear();
}

bool CBlockLocator::IsNull()
{
	return vHave.empty();
}

void CBlockLocator::Set(const CBlockIndex* pindex)
{
	int nStep = 1;
	
	vHave.clear();
	
	while (pindex)
	{
		vHave.push_back(pindex->GetBlockHash());

		// v2.0.0.9 rc11: stop here rather than walk on to genesis
		// (LOCATOR_MAX_ENTRIES above).  The genesis hash is still added below.
		if (vHave.size() >= LOCATOR_MAX_ENTRIES)
		{
			break;
		}

		// Exponentially larger steps back
		for (int i = 0; pindex && i < nStep; i++)
		{
			pindex = pindex->pprev;
		}
		
		if (vHave.size() > 10)
		{
			nStep *= 2;
		}
	}
	
	vHave.push_back(Params().HashGenesisBlock());
}

int CBlockLocator::GetDistanceBack()
{
	// Retrace how far back it was in the sender's branch
	int nDistance = 0;
	int nStep = 1;
	
	for(const uint256& hash : vHave)
	{
		std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hash);
		
		if (mi != mapBlockIndex.end())
		{
			CBlockIndex* pindex = (*mi).second;
			
			if (pindex->IsInMainChain())
			{
				return nDistance;
			}
		}
		
		nDistance += nStep;
		
		if (nDistance > 10)
		{
			nStep *= 2;
		}
	}
	
	return nDistance;
}

CBlockIndex* CBlockLocator::GetBlockIndex()
{
	// Find the first block the caller has in the main chain
	for(const uint256& hash : vHave)
	{
		std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hash);
		
		if (mi != mapBlockIndex.end())
		{
			CBlockIndex* pindex = (*mi).second;
			
			if (pindex->IsInMainChain())
			{
				return pindex;
			}
		}
	}
	
	return pindexGenesisBlock;
}

uint256 CBlockLocator::GetBlockHash()
{
	// Find the first block the caller has in the main chain
	for(const uint256& hash : vHave)
	{
		std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hash);
		
		if (mi != mapBlockIndex.end())
		{
			CBlockIndex* pindex = (*mi).second;
			
			if (pindex->IsInMainChain())
			{
				return hash;
			}
		}
	}
	
	return Params().HashGenesisBlock();
}

int CBlockLocator::GetHeight()
{
	CBlockIndex* pindex = GetBlockIndex();
	
	if (!pindex)
	{
		return 0;
	}
	
	return pindex->nHeight;
}

