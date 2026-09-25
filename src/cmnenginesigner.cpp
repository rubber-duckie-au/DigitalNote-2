#include "uint/uint256.h"
#include "cscript.h"
#include "ctransaction.h"
#include "ckey.h"
#include "cpubkey.h"
#include "ctxin.h"
#include "ctxout.h"
#include "cblockindex.h"
#include "chashwriter.h"
#include "main.h"
#include "main_extern.h"
#include "mining.h"
#include "util.h"
#include "script.h"
#include "cdigitalnotesecret.h"
#include "cnodestination.h"
#include "ckeyid.h"
#include "cscriptid.h"
#include "cstealthaddress.h"
#include "ui_translate.h"
#include "enums/serialize_type.h"

#include "cmnenginesigner.h"

// v2.0.0.9 W-17: same lookup, but it says WHY it failed.
//
// GetTransaction() returning false means the collateral is not in the chain we
// have -- which during sync is the normal case and tells us nothing about the
// peer that sent the announcement.  Only a transaction we CAN read, that does
// not pay the claimed key, is evidence against the sender.
CMNengineSigner::VinPubkeyResult CMNengineSigner::CheckVinPubkeyAssociation(CTxIn& vin, CPubKey& pubkey)
{
	CScript payee2;
	CTransaction txVin;
	uint256 hash;

	payee2 = GetScriptForDestination(pubkey.GetID());

	if(!GetTransaction(vin.prevout.hash, txVin, hash))
	{
		return VINPUBKEY_TX_NOT_FOUND;
	}

	for(CTxOut out : txVin.vout)
	{
		if(out.nValue == MasternodeCollateral(pindexBest->nHeight)*COIN)
		{
			if(out.scriptPubKey == payee2)
			{
				return VINPUBKEY_MATCH;
			}
		}
	}

	return VINPUBKEY_MISMATCH;
}

bool CMNengineSigner::IsVinAssociatedWithPubkey(CTxIn& vin, CPubKey& pubkey)
{
	CScript payee2;
	CTransaction txVin;
	uint256 hash;

	payee2 = GetScriptForDestination(pubkey.GetID());

	//if(GetTransaction(vin.prevout.hash, txVin, hash, true)){
	if(GetTransaction(vin.prevout.hash, txVin, hash))
	{
		for(CTxOut out : txVin.vout)
		{
			if(out.nValue == MasternodeCollateral(pindexBest->nHeight)*COIN)
			{
				if(out.scriptPubKey == payee2)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool CMNengineSigner::SetKey(const std::string &strSecret, std::string& errorMessage, CKey& key, CPubKey& pubkey)
{
	CDigitalNoteSecret vchSecret;
	bool fGood = vchSecret.SetString(strSecret);

	if (!fGood)
	{
		errorMessage = ui_translate("");//NOTE: previous message contents - Invalid private key.
		
		return false;
	}

	key = vchSecret.GetKey();
	pubkey = key.GetPubKey();

	LogPrint("masternode", "CMNengineSetKey(): SetKey now set successfully \n");

	return true;
}

bool CMNengineSigner::SignMessage(const std::string &strMessage, std::string& errorMessage, std::vector<unsigned char>& vchSig, CKey key)
{
	CHashWriter ss(SER_GETHASH, 0);
	ss << strMessageMagic;
	ss << strMessage;

	if (!key.SignCompact(ss.GetHash(), vchSig))
	{
		errorMessage = ui_translate("Signing failed.");
		
		return false;
	}

	return true;
}

bool CMNengineSigner::VerifyMessage(CPubKey pubkey, std::vector<unsigned char>& vchSig, const std::string &strMessage,
		std::string& errorMessage)
{
	CHashWriter ss(SER_GETHASH, 0);
	
	ss << strMessageMagic;
	ss << strMessage;

	CPubKey pubkey2;
	
	if (!pubkey2.RecoverCompact(ss.GetHash(), vchSig))
	{
		errorMessage = ui_translate("Error recovering public key.");
		
		return false;
	}

	if (fDebug && (pubkey2.GetID() != pubkey.GetID()))
	{
		LogPrintf("CMNengineSigner::VerifyMessage -- keys don't match: %s %s\n", pubkey2.GetID().ToString(), pubkey.GetID().ToString());
	}

	return (pubkey2.GetID() == pubkey.GetID());
}

