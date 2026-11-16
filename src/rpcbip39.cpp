#include <bip39.h>
#include <bip39/mnemonic.h>
#include <bip39/seed.h>
#include <bip39/bip39_passphrase.h>

#include "util.h"
#include "json/json_spirit_value.h"
#include "enums/rpcerrorcode.h"
#include "rpcprotocol.h"
#include "rpcserver.h"
#include "ckey.h"
#include "cdigitalnotesecret.h"
#include "cwallet.h"
#include "init.h"
#include <openssl/crypto.h>

json_spirit::Value getrecoveryphrase(const json_spirit::Array& params, bool fHelp)
{
	if (fHelp || params.size() != 1)
	{
		throw std::runtime_error(
			"getrecoveryphrase \"passphrase\"\n"
			"\n"
			"Returns the 24-word BIP39 recovery phrase for this wallet -- the same\n"
			"phrase the GUI shows (Settings -> Recovery Phrase).\n"
			"The wallet must be unlocked before calling this command.\n"
			"The phrase comes from the wallet's master key: it does NOT change when\n"
			"you change the wallet password, only when the phrase is rotated.\n"
			"\n"
			"IMPORTANT: Keep this phrase secret. Anyone with this phrase and\n"
			"           your wallet.dat can access your funds.\n"
			"\n"
			"NOTE: For wallets encrypted with an older version of DigitalNote,\n"
			"      use the GUI first: Settings -> Recovery Phrase\n"
			"      to complete the one-time upgrade.\n"
			"\n"
			"Arguments:\n"
			"  passphrase  (string, required) Your wallet encryption password\n"
			"\n"
			"Result:\n"
			"  {\n"
			"    \"phrase\" : \"word1 word2 ... word24\"\n"
			"  }\n"
			"\n"
			"Examples:\n"
			+ HelpExampleCli("getrecoveryphrase", "\"my wallet password\"")
			+ HelpExampleRpc("getrecoveryphrase", "\"my wallet password\"")
		);
	}

	if (!pwalletMain)
		throw JSONRPCError(RPC_WALLET_ERROR, "Wallet not loaded.");

	if (!pwalletMain->IsCrypted())
		throw JSONRPCError(RPC_WALLET_WRONG_ENC_STATE,
			"Error: Wallet is not encrypted. Encrypt your wallet first.");

	// Wallet must be unlocked - we do not unlock it here
	if (pwalletMain->IsLocked())
		throw JSONRPCError(RPC_WALLET_UNLOCK_NEEDED,
			"Error: Wallet is locked. "
			"Please unlock with walletpassphrase before calling getrecoveryphrase.");

	// Validate passphrase
	SecureString strPassphrase;
	strPassphrase.reserve(100);
	strPassphrase = params[0].get_str().c_str();

	if (strPassphrase.empty())
		throw JSONRPCError(RPC_INVALID_PARAMETER, "Passphrase cannot be empty.");

	// Verify the password is correct
	if (!pwalletMain->VerifyPassphrase(strPassphrase))
		throw JSONRPCError(RPC_WALLET_PASSPHRASE_INCORRECT,
			"Error: The wallet passphrase entered was incorrect.");

	OPENSSL_cleanse(const_cast<char*>(strPassphrase.data()), strPassphrase.size());

	// v2.0.0.9 rc9 (review 5.1, A7): the wallet must actually hold a recovery-
	// phrase master key.  The old check (HasRecoveryPhraseFlag) is also set by
	// EncryptWallet without creating that key, so it let wallets with no phrase
	// through.  The GUI (Settings -> Recovery Phrase) performs the one-time
	// upgrade that creates it.
	if (!pwalletMain->HasMnemonicMasterKey())
		throw JSONRPCError(RPC_WALLET_ERROR,
			"Error: This wallet has no recovery phrase yet. "
			"Use the GUI: Settings -> Recovery Phrase to complete the one-time upgrade. "
			"Note: Multiple recovery phrases are not supported - one phrase per wallet.");

	// v2.0.0.9 rc9 (review 5.1, A7): return the wallet's REAL phrase.  It used
	// to derive one from the password (BIP39Passphrase::mnemonicFromPassphrase,
	// the deprecated "D1" scheme), which unlocks nothing: the stored phrase key
	// is derived from the master key ("D2", mnemonicFromVMasterKey), exactly as
	// GetCurrentMnemonic does for the GUI.
	SecureString mnemonic;

	if (!pwalletMain->GetCurrentMnemonic(mnemonic))
		throw JSONRPCError(RPC_WALLET_ERROR, "Failed to derive the recovery phrase.");

	json_spirit::Object result;
	result.push_back(json_spirit::Pair("phrase",
		std::string(mnemonic.begin(), mnemonic.end())));
	OPENSSL_cleanse(const_cast<char*>(mnemonic.data()), mnemonic.size());

	return result;
}
