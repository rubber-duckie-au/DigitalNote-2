#ifndef CSTEALTHADDRESS_H
#define CSTEALTHADDRESS_H

#include <string>

#include "stealth.h"
#include "serialize.h"

class CPubKey;

class CStealthAddress
{
public:
    uint8_t options;
    ec_point scan_pubkey;
    ec_point spend_pubkey;
    //std::vector<ec_point> spend_pubkeys;
    size_t number_signatures;
    stealth_prefix prefix;
    mutable std::string label;
    data_chunk scan_secret;
    data_chunk spend_secret;
    
	CStealthAddress();
	
	IMPLEMENT_SERIALIZE
    (
        READWRITE(this->options);
        READWRITE(this->scan_pubkey);
        READWRITE(this->spend_pubkey);
        READWRITE(this->label);
        READWRITE(this->scan_secret);
        READWRITE(this->spend_secret);
    );
	
    bool SetEncoded(const std::string& encodedAddress);
    std::string Encoded() const;
    int SetScanPubKey(CPubKey pk);
    bool operator<(const CStealthAddress& y) const;
};

#endif // CSTEALTHADDRESS_H
