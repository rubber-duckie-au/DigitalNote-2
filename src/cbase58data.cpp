#include <cstring>

#include "base58.h"
#include "support/cleanse.h"

#include "cbase58data.h"

CBase58Data::CBase58Data()
{
    vchVersion.clear();
    vchData.clear();
}

void CBase58Data::SetData(const std::vector<unsigned char> &vchVersionIn, const void* pdata, size_t nSize)
{
    vchVersion = vchVersionIn;
    vchData.resize(nSize);
	
    if (!vchData.empty())
	{
        memcpy(&vchData[0], pdata, nSize);
	}
}

void CBase58Data::SetData(const std::vector<unsigned char> &vchVersionIn, const unsigned char *pbegin,
		const unsigned char *pend)
{
    SetData(vchVersionIn, (void*)pbegin, pend - pbegin);
}

bool CBase58Data::SetString(const char* psz, unsigned int nVersionBytes)
{
    std::vector<unsigned char> vchTemp;
    bool rc58 = DecodeBase58Check(psz, vchTemp);
	
    if ((!rc58) || (vchTemp.size() < nVersionBytes))
	{
        vchData.clear();
        vchVersion.clear();
        
		return false;
    }
	
    vchVersion.assign(vchTemp.begin(), vchTemp.begin() + nVersionBytes);
    vchData.resize(vchTemp.size() - nVersionBytes);
    
	if (!vchData.empty())
	{
        memcpy(&vchData[0], &vchTemp[nVersionBytes], vchData.size());
	}
	
    memory_cleanse(&vchTemp[0], vchData.size());
    
	return true;
}

bool CBase58Data::SetString(const std::string& str)
{
    return SetString(str.c_str());
}

std::string CBase58Data::ToString() const
{
    std::vector<unsigned char> vch = vchVersion;
	
    vch.insert(vch.end(), vchData.begin(), vchData.end());
    
	return EncodeBase58Check(vch);
}

int CBase58Data::CompareTo(const CBase58Data& b58) const
{
    if (vchVersion < b58.vchVersion)
	{
		return -1;
	}
	
    if (vchVersion > b58.vchVersion)
	{
		return 1;
	}
	
    if (vchData < b58.vchData)
	{
		return -1;
	}
	
    if (vchData > b58.vchData)
	{
		return 1;
	}
	
    return 0;
}

bool CBase58Data::operator==(const CBase58Data& b58) const
{
	return CompareTo(b58) == 0;
}

bool CBase58Data::operator<=(const CBase58Data& b58) const
{
	return CompareTo(b58) <= 0;
}

bool CBase58Data::operator>=(const CBase58Data& b58) const
{
	return CompareTo(b58) >= 0;
}

bool CBase58Data::operator< (const CBase58Data& b58) const
{
	return CompareTo(b58) <  0;
}

bool CBase58Data::operator> (const CBase58Data& b58) const
{
	return CompareTo(b58) >  0;
}

