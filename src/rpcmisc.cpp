#include "compat.h"

#include <stdint.h>

#include "net/proxytype.h"
#include "net/cservice.h"
#include "net.h"
#include "netbase.h"
#include "util.h"
#include "version.h"
#include "init.h"
#include "rpcserver.h"
#include "main_extern.h"
#include "cblockindex.h"
#include "chainparams.h"
#include "cdigitalnoteaddress.h"
#include "cnodestination.h"
#include "cstealthaddress.h"
#include "cscriptid.h"
#include "ckeyid.h"
#include "script.h"
#include "enums/rpcerrorcode.h"
#include "chashwriter.h"
#include "enums/serialize_type.h"
#include "spork.h"
#include "csporkmessage.h"
#include "rpcprotocol.h"

#ifdef ENABLE_WALLET
#include "walletdb.h"
#include "cwallet.h"
#include "wallet.h"

#include "describeaddressvisitor.h"
#endif

json_spirit::Value getinfo(const json_spirit::Array& params, bool fHelp)
{
	if (fHelp || params.size() != 0)
	{
		throw std::runtime_error(
			"getinfo\n"
			"Returns an object containing various state info."
		);
	}

	proxyType proxy;
	GetProxy(NET_IPV4, proxy);

	json_spirit::Object obj, diff;
	obj.push_back(json_spirit::Pair("version", FormatFullVersion()));
	obj.push_back(json_spirit::Pair("protocolversion",(int)PROTOCOL_VERSION));

#ifdef ENABLE_WALLET

	if (pwalletMain)
	{
		obj.push_back(json_spirit::Pair("walletversion", pwalletMain->GetVersion()));
		obj.push_back(json_spirit::Pair("balance", ValueFromAmount(pwalletMain->GetBalance())));
		obj.push_back(json_spirit::Pair("newmint", ValueFromAmount(pwalletMain->GetNewMint())));
		obj.push_back(json_spirit::Pair("stake", ValueFromAmount(pwalletMain->GetStake())));
	}

#endif // ENABLE_WALLET

	obj.push_back(json_spirit::Pair("blocks", (int)nBestHeight));
	obj.push_back(json_spirit::Pair("timeoffset", (int64_t)GetTimeOffset()));
	obj.push_back(json_spirit::Pair("moneysupply", ValueFromAmount(pindexBest->nMoneySupply)));
	obj.push_back(json_spirit::Pair("connections", (int)vNodes.size()));
	obj.push_back(json_spirit::Pair("proxy", (proxy.first.IsValid() ? proxy.first.ToStringIPPort() : std::string())));
	obj.push_back(json_spirit::Pair("ip", GetLocalAddress(NULL).ToStringIP()));

	diff.push_back(json_spirit::Pair("proof-of-work", GetDifficulty()));
	diff.push_back(json_spirit::Pair("proof-of-stake", GetDifficulty(GetLastBlockIndex(pindexBest, true))));

	obj.push_back(json_spirit::Pair("difficulty", diff));
	obj.push_back(json_spirit::Pair("testnet", TestNet()));

#ifdef ENABLE_WALLET

	if (pwalletMain)
	{
		obj.push_back(json_spirit::Pair("keypoololdest", (int64_t)pwalletMain->GetOldestKeyPoolTime()));
		obj.push_back(json_spirit::Pair("keypoolsize", (int)pwalletMain->GetKeyPoolSize()));
	}

	obj.push_back(json_spirit::Pair("paytxfee", ValueFromAmount(nTransactionFee)));
	obj.push_back(json_spirit::Pair("mininput", ValueFromAmount(nMinimumInputValue)));

	if (pwalletMain && pwalletMain->IsCrypted())
	{
		obj.push_back(json_spirit::Pair("unlocked_until", (int64_t)nWalletUnlockTime));
	}
	
#endif // ENABLE_WALLET

	obj.push_back(json_spirit::Pair("errors", GetWarnings("statusbar")));

	return obj;
}

json_spirit::Value validateaddress(const json_spirit::Array& params, bool fHelp)
{
	if (fHelp || params.size() != 1)
	{
		throw std::runtime_error(
			"validateaddress <DigitalNote>\n"
			"Return information about <DigitalNote>."
		);
	}

	CDigitalNoteAddress address(params[0].get_str());
	bool isValid = address.IsValid();

	json_spirit::Object ret;
	ret.push_back(json_spirit::Pair("isvalid", isValid));

	if (isValid)
	{
		CTxDestination dest = address.Get();
		std::string currentAddress = address.ToString();
		
		ret.push_back(json_spirit::Pair("address", currentAddress));
#ifdef ENABLE_WALLET
		
		isminetype mine = pwalletMain ? IsMine(*pwalletMain, dest) : ISMINE_NO;
		
		ret.push_back(json_spirit::Pair("ismine", (mine & ISMINE_SPENDABLE) ? true : false));
		
		if (mine != ISMINE_NO) {
			ret.push_back(json_spirit::Pair("iswatchonly", (mine & ISMINE_WATCH_ONLY) ? true: false));
			
			json_spirit::Object detail = boost::apply_visitor(DescribeAddressVisitor(mine), dest);
			
			ret.insert(ret.end(), detail.begin(), detail.end());
		}
		
		if (pwalletMain && pwalletMain->mapAddressBook.count(dest))
		{
			ret.push_back(json_spirit::Pair("account", pwalletMain->mapAddressBook[dest]));
		}
#endif // ENABLE_WALLET
	}

	return ret;
}

json_spirit::Value validatepubkey(const json_spirit::Array& params, bool fHelp)
{
	if (fHelp || !params.size() || params.size() > 2)
	{
		throw std::runtime_error(
			"validatepubkey <DigitalNotepubkey>\n"
			"Return information about <DigitalNotepubkey>."
		);
	}

	std::vector<unsigned char> vchPubKey = ParseHex(params[0].get_str());
	CPubKey pubKey(vchPubKey);

	bool isValid = pubKey.IsValid();
	bool isCompressed = pubKey.IsCompressed();
	CKeyID keyID = pubKey.GetID();

	CDigitalNoteAddress address;
	address.Set(keyID);

	json_spirit::Object ret;
	ret.push_back(json_spirit::Pair("isvalid", isValid));

	if (isValid)
	{
		CTxDestination dest = address.Get();
		std::string currentAddress = address.ToString();
		
		ret.push_back(json_spirit::Pair("address", currentAddress));
		ret.push_back(json_spirit::Pair("iscompressed", isCompressed));
#ifdef ENABLE_WALLET
		
		isminetype mine = pwalletMain ? IsMine(*pwalletMain, dest) : ISMINE_NO;
		
		ret.push_back(json_spirit::Pair("ismine", (mine & ISMINE_SPENDABLE) ? true : false));
		
		if (mine != ISMINE_NO)
		{
			ret.push_back(json_spirit::Pair("iswatchonly", (mine & ISMINE_WATCH_ONLY) ? true: false));
			
			json_spirit::Object detail = boost::apply_visitor(DescribeAddressVisitor(mine), dest);
			
			ret.insert(ret.end(), detail.begin(), detail.end());
		}
		
		if (pwalletMain && pwalletMain->mapAddressBook.count(dest))
		{
			ret.push_back(json_spirit::Pair("account", pwalletMain->mapAddressBook[dest]));
		}
#endif // ENABLE_WALLET
	}

	return ret;
}

json_spirit::Value verifymessage(const json_spirit::Array& params, bool fHelp)
{
	if (fHelp || params.size() != 3)
	{
		throw std::runtime_error(
			"verifymessage <DigitalNote> <signature> <message>\n"
			"Verify a signed message"
		);
	}

	std::string strAddress = params[0].get_str();
	std::string strSign = params[1].get_str();
	std::string strMessage = params[2].get_str();

	CDigitalNoteAddress addr(strAddress);
	if (!addr.IsValid())
	{
		throw JSONRPCError(RPC_TYPE_ERROR, "Invalid address");
	}

	CKeyID keyID;
	if (!addr.GetKeyID(keyID))
	{
		throw JSONRPCError(RPC_TYPE_ERROR, "Address does not refer to key");
	}

	bool fInvalid = false;
	std::vector<unsigned char> vchSig = DecodeBase64(strSign.c_str(), &fInvalid);

	if (fInvalid)
	{
		throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Malformed base64 encoding");
	}

	CHashWriter ss(SER_GETHASH, 0);
	ss << strMessageMagic;
	ss << strMessage;

	CPubKey pubkey;
	if (!pubkey.RecoverCompact(ss.GetHash(), vchSig))
	{
		return false;
	}

	return (pubkey.GetID() == keyID);
}

/*
	Used for updating/reading spork settings on the network
*/
json_spirit::Value spork(const json_spirit::Array& params, bool fHelp)
{
	if(params.size() == 1 && params[0].get_str() == "show")
	{
		std::map<int, CSporkMessage>::iterator it = mapSporksActive.begin();

		json_spirit::Object ret;
		while(it != mapSporksActive.end())
		{
			ret.push_back(json_spirit::Pair(sporkManager.GetSporkNameByID(it->second.nSporkID), it->second.nValue));
			it++;
		}
		
		return ret;
	}
	else if (params.size() == 2)
	{
		int nSporkID = sporkManager.GetSporkIDByName(params[0].get_str());
		if(nSporkID == -1)
		{
			return "Invalid spork name";
		}

		// SPORK VALUE
		int64_t nValue = params[1].get_int();

		//broadcast new spork
		if(sporkManager.UpdateSpork(nSporkID, nValue))
		{
			return "success";
		}
		else
		{
			return "failure";
		}

	}

	throw std::runtime_error(
		"spork <name> [<value>]\n"
		"<name> is the corresponding spork name, or 'show' to show all current spork settings"
		"<value> is a epoch datetime to enable or disable spork"
		+ HelpRequiringPassphrase()
	);
}

// v2.0.0.9 TODO 3.9: listdebugcategories -- debug-category discoverability.
//
// Closes the last operator-experience gap left by v2.0.0.8's CW14 work.  CW14
// made -debug=all and -debug=1 behave as wildcards and documented the alias
// forms in --help; what remained was that an operator had no way to ask a
// RUNNING node which categories exist or which are currently in force.
//
// Mirrors the intent of Bitcoin Core's `logging` RPC.  Read-only: it reports
// state, it does not change it.  No consensus surface.
//
// The `unknown_requested` field is the operationally useful part.  Category
// names are matched by exact string against the LogPrint("category", ...) call
// sites, so a plausible-looking typo -- -debug=masternodes for masternode, or
// -debug=network for net -- silently produces no output at all, with fDebug
// still true.  That failure is invisible today; listing the unmatched names
// makes it obvious.
//
// >>> MAINTENANCE: vDebugCategories below is a HARDCODED list and cannot be
// >>> derived at runtime (the categories are string literals at their call
// >>> sites).  WHEN YOU ADD A NEW LogPrint("newcat", ...) CATEGORY ANYWHERE,
// >>> ADD IT HERE TOO.  The list was derived on 2026-07-28 by:
// >>>     grep -rhoE 'LogPrint\("[a-z0-9-]+"' --include=*.cpp --include=*.h src/
// >>> Re-run that to audit for drift.
json_spirit::Value listdebugcategories(const json_spirit::Array& params, bool fHelp)
{
	if (fHelp || params.size() != 0)
	{
		throw std::runtime_error(
			"listdebugcategories\n"
			"\nLists the debug logging categories this build knows about, and which\n"
			"are currently active on this node.  Read-only; does not change logging.\n"
			"\nCategories are selected with -debug=<category> (repeatable).  The forms\n"
			"-debug, -debug=all and -debug=1 enable every category.\n"
			"\nResult:\n"
			"{\n"
			"  \"debug_enabled\": bool,        (boolean) true if any -debug form is in force\n"
			"  \"wildcard\": bool,             (boolean) true if all categories are enabled\n"
			"  \"active\": [ \"cat\", ... ],     (array) categories currently producing output\n"
			"  \"known\": [ \"cat\", ... ],      (array) every category this build supports\n"
			"  \"unknown_requested\": [ ... ]  (array) -debug values matching no known category\n"
			"}\n"
			"\nExamples:\n"
			+ HelpExampleCli("listdebugcategories", "")
			+ HelpExampleRpc("listdebugcategories", "")
		);
	}

	// Every category appearing in a LogPrint() call site.  See the maintenance
	// note above before editing.
	static const char* vDebugCategories[] = {
		"addrman",
		"alert",
		"checkblock",
		"coinage",
		"coinstake",
		"creation",
		"db",
		"init",
		"instantx",
		"lock",
		"masternode",
		"mempool",
		"mnengine",
		"net",
		"rand",
		"retarget",
		"rpc",
		"selectcoins",
		"smsg",
		"stakemodifier",
		"webwallet"
	};
	const size_t nKnown = sizeof(vDebugCategories) / sizeof(vDebugCategories[0]);

	std::set<std::string> setKnown;

	for (size_t i = 0; i < nKnown; i++)
	{
		setKnown.insert(std::string(vDebugCategories[i]));
	}

	// What the operator actually asked for on the command line / config file.
	const std::vector<std::string>& vRequested = mapMultiArgs["-debug"];
	std::set<std::string> setRequested(vRequested.begin(), vRequested.end());

	// Wildcard forms, matching LogAcceptCategory() in util.cpp exactly:
	//   -debug      -> ""      (legacy)
	//   -debug=all  -> "all"   (CW14)
	//   -debug=1    -> "1"     (CW14)
	// Keep this in lockstep with util.cpp; if a new alias is added there it
	// must be added here or this RPC will misreport.
	bool fWildcard = setRequested.count(std::string("")) > 0 ||
	                 setRequested.count(std::string("all")) > 0 ||
	                 setRequested.count(std::string("1")) > 0;

	json_spirit::Array arrKnown;
	json_spirit::Array arrActive;

	for (size_t i = 0; i < nKnown; i++)
	{
		const std::string strCategory(vDebugCategories[i]);

		arrKnown.push_back(strCategory);

		// A category is active only if debugging is on at all.  fDebug is set
		// in init.cpp from the presence of -debug; without it LogAcceptCategory
		// short-circuits and nothing is emitted regardless of category.
		if (fDebug && (fWildcard || setRequested.count(strCategory) > 0))
		{
			arrActive.push_back(strCategory);
		}
	}

	// Requested values that match no known category AND are not a wildcard
	// alias.  These are almost always typos and produce silent no-ops today.
	json_spirit::Array arrUnknown;

	for (std::set<std::string>::const_iterator it = setRequested.begin(); it != setRequested.end(); ++it)
	{
		if (it->empty() || *it == "all" || *it == "1")
		{
			continue;
		}

		if (setKnown.count(*it) == 0)
		{
			arrUnknown.push_back(*it);
		}
	}

	json_spirit::Object objResult;

	objResult.push_back(json_spirit::Pair("debug_enabled", fDebug));
	objResult.push_back(json_spirit::Pair("wildcard", fWildcard));
	objResult.push_back(json_spirit::Pair("active", arrActive));
	objResult.push_back(json_spirit::Pair("known", arrKnown));
	objResult.push_back(json_spirit::Pair("unknown_requested", arrUnknown));

	return objResult;
}
