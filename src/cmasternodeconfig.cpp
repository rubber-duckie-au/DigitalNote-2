#include "compat.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "net.h"
#include "util.h"
#include "base58.h"
#include "cmasternodeconfigentry.h"

#include "cmasternodeconfig.h"

CMasternodeConfig::CMasternodeConfig()
{
	entries = std::vector<CMasternodeConfigEntry>();
}

std::vector<CMasternodeConfigEntry>& CMasternodeConfig::getEntries()
{
	return entries;
}

void CMasternodeConfig::add(const std::string &alias, const std::string &ip, const std::string &privKey,
		const std::string &txHash, const std::string &outputIndex)
{
	CMasternodeConfigEntry cme(alias, ip, privKey, txHash, outputIndex);

	entries.push_back(cme);
}

bool CMasternodeConfig::read(boost::filesystem::path path)
{
	boost::filesystem::ifstream streamConfig(GetMasternodeConfigFile());

	if (!streamConfig.good())
	{
		return true; // No masternode.conf file is OK
	}

	// v2.0.0.9 rc9 (review 3.11 / 2.#19, A18): trim whitespace and a Windows
	// '\r', skip blank lines and '#' comment lines, and skip (with a log line) a
	// malformed line instead of silently dropping it AND every line after it.
	// Returns false if any line was malformed, so init can warn.
	bool fAllParsed = true;

	for(std::string line; std::getline(streamConfig, line); )
	{
		const std::string strWhitespace = " \t\r\n";
		const size_t nFirst = line.find_first_not_of(strWhitespace);

		if (nFirst == std::string::npos)
		{
			continue;	// blank (or whitespace-only) line
		}

		line = line.substr(nFirst, line.find_last_not_of(strWhitespace) - nFirst + 1);

		if (line[0] == '#')
		{
			continue;	// comment line
		}

		std::istringstream iss(line);
		std::string alias, ip, privKey, txHash, outputIndex;

		if (!(iss >> alias >> ip >> privKey >> txHash >> outputIndex))
		{
			LogPrintf("Could not parse masternode.conf line, skipping it: %s\n", line.c_str());

			fAllParsed = false;

			continue;
		}

		add(alias, ip, privKey, txHash, outputIndex);
	}

	streamConfig.close();

	return fAllParsed;
}

