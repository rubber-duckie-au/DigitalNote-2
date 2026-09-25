#ifndef CMNENGINESIGNER_H
#define CMNENGINESIGNER_H

#include <string>
#include <vector>

class CTxIn;
class CPubKey;
class CKey;

/** Helper object for signing and checking signatures
 */
class CMNengineSigner
{
public:
	/// Is the inputs associated with this public key? (and there is 10000 XDN - checking if valid masternode)
	// v2.0.0.9 W-17: the boolean form cannot distinguish "the collateral
	// transaction is not in OUR chain yet" from "it is, and it does not match".
	// The caller banned on both, so a syncing node banned honest peers for its
	// OWN missing data.  Use CheckVinPubkeyAssociation() for anything that
	// penalises a peer.
	enum VinPubkeyResult
	{
		VINPUBKEY_MATCH = 0,        // found, and it pays the claimed key
		VINPUBKEY_MISMATCH = 1,     // found, and it does NOT -- the peer is wrong
		VINPUBKEY_TX_NOT_FOUND = 2  // not in our chain -- says nothing about the peer
	};

	VinPubkeyResult CheckVinPubkeyAssociation(CTxIn& vin, CPubKey& pubkey);
	bool IsVinAssociatedWithPubkey(CTxIn& vin, CPubKey& pubkey);
	/// Set the private/public key values, returns true if successful
	bool SetKey(const std::string &strSecret, std::string& errorMessage, CKey& key, CPubKey& pubkey);
	/// Sign the message, returns true if successful
	bool SignMessage(const std::string &strMessage, std::string& errorMessage, std::vector<unsigned char>& vchSig, CKey key);
	/// Verify the message, returns true if succcessful
	bool VerifyMessage(CPubKey pubkey, std::vector<unsigned char>& vchSig, const std::string &strMessage, std::string& errorMessage);
};

#endif // CMNENGINESIGNER_H
