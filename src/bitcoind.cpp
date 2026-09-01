#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

#include "rpcclient.h"
#include "init.h"
#include "util.h"
#include "main_extern.h"
#include "chainparams.h"
#include "noui.h"
#include "ui_translate.h"
#include "fork.h"

void WaitForShutdown(boost::thread_group* threadGroup)
{
	bool fShutdown = ShutdownRequested();

	// Tell the main threads to shutdown.
	while (!fShutdown)
	{
		MilliSleep(200);
		fShutdown = ShutdownRequested();
	}

	if (threadGroup)
	{
		threadGroup->interrupt_all();
		threadGroup->join_all();
	}
}

//////////////////////////////////////////////////////////////////////////////
//
// Start
//
bool AppInit(int argc, char* argv[])
{
	boost::thread_group threadGroup;
	bool fRet = false;

	fHaveGUI = false;

	try
	{
		//
		// Parameters
		//
		// If Qt is used, parameters/bitcoin.conf are parsed in qt/bitcoin.cpp's main()
		ParseParameters(argc, argv);

		// v2.0.0.9: select the network HERE, from the command line only.
		//
		// WHY THIS IS THE RIGHT PLACE.  Params() defaults to MAINNET at static-init
		// (chainparams.cpp:19) -- SelectParams does not establish the network, it
		// CORRECTS a default that is already silently in force.  Until it runs,
		// anything asking Params() gets "mainnet" with no warning.
		//
		// That matters most for logging.  LogPrintStr -> call_once(DebugPrintInit)
		// -> GetDataDir() -> Params(), and DebugPrintInit fopen()s debug.log ONCE.
		// A single LogPrintf before the network is selected binds debug.log to the
		// MAINNET directory for the whole process, whatever -testnet says.  The
		// path cache self-corrects; an already-open file handle does not.
		//
		// BEFORE ReadConfigFile, deliberately.  GetNetworkConfigDir() (util.cpp:1575)
		// picks which conf to read using GetBoolArg("-testnet") straight from
		// mapArgs, and ReadConfigFile then INJECTS conf entries back into mapArgs
		// (util.cpp:1809).  Selecting AFTER the read means SelectParams sees those
		// injected values while GetNetworkConfigDir did not -- so a testnet=1 line
		// in the MAINNET conf would load the mainnet conf and then select testnet.
		// Running first makes both read the same inputs at the same instant.
		//
		// CONSEQUENCE, and it is intended: the network is chosen by the -testnet /
		// -regtest COMMAND-LINE switch only.  A testnet= line in a conf file is
		// REDUNDANT and does nothing -- which is already what util.cpp:1571-1574
		// documents, and is now actually true.
		//
		// Takes no locks and runs single-threaded: GetBoolArg is a mapArgs read and
		// SelectParams assigns a pointer.  No thread exists this early.
		if (!SelectParamsFromCommandLine())
		{
			fprintf(stderr, "Error: invalid combination of -regtest and -testnet.\n");
			return false;
		}

		
		if (!boost::filesystem::is_directory(GetDataDir(false)))
		{
			fprintf(stderr, "Error: Specified directory does not exist\n");
			
			Shutdown();
		}
		
		// v2.0.0.8 testnet-conf-generator: generate a default conf in the
		// network-specific data directory if absent, BEFORE ReadConfigFile
		// so the freshly-generated conf is read on this same run.  The
		// network is known here from the -testnet command-line flag
		// (ParseParameters has run); GenerateDefaultConfigFile and
		// GetConfigFile both resolve the network-specific path the same way.
		GenerateDefaultConfigFile();

		ReadConfigFile(mapArgs, mapMultiArgs);

		if (mapArgs.count("-?") || mapArgs.count("--help"))
		{
			// First part of help message is specific to bitcoind / RPC client
			std::string strUsage = ui_translate("DigitalNote version") + " " + FormatFullVersion() + "\n\n" +
				ui_translate("Usage:") + "\n" +
				  "  DigitalNoted [options]                     " + "\n" +
				  "  DigitalNoted [options] <command> [params]  " + ui_translate("Send command to -server or DigitalNoted") + "\n" +
				  "  DigitalNoted [options] help                " + ui_translate("List commands") + "\n" +
				  "  DigitalNoted [options] help <command>      " + ui_translate("Get help for a command") + "\n";

			strUsage += "\n" + HelpMessage();

			fprintf(stdout, "%s", strUsage.c_str());
			
			return false;
		}

		// Command-line RPC
		for (int i = 1; i < argc; i++)
		{
			if (!IsSwitchChar(argv[i][0]) && !boost::algorithm::istarts_with(argv[i], "DigitalNote:"))
			{
				fCommandLine = true;
			}
		}
		
		if (fCommandLine)
		{
			// v2.0.0.9: the SelectParamsFromCommandLine() that was here is REMOVED.
			// It is now done once, immediately after ParseParameters() above.
			//
			// Leaving it would have been worse than redundant: by this point
			// ReadConfigFile() has injected conf entries into mapArgs
			// (util.cpp:1809), so a testnet= line in the MAINNET conf would
			// re-select TESTNET here and the RPC client would talk to the wrong
			// network -- the exact inconsistency the earlier placement prevents.
			int ret = CommandLineRPC(argc, argv);
			
			exit(ret);
		}
		
	#if !WIN32
		fDaemon = GetBoolArg("-daemon", false);
		
		if (fDaemon)
		{
			// Daemonize
			pid_t pid = fork();
			
			if (pid < 0)
			{
				fprintf(stderr, "Error: fork() returned %d errno %d\n", pid, errno);
				
				return false;
			}
			
			if (pid > 0) // Parent process, pid is child process id
			{
				CreatePidFile(GetPidFile(), pid);
				
				return true;
			}
			
			// Child process falls through to rest of initialization
			pid_t sid = setsid();
			
			if (sid < 0)
			{
				fprintf(stderr, "Error: setsid() returned %d errno %d\n", sid, errno);
			}
		}
	#endif

		fRet = AppInit2(threadGroup);
	}
	catch (std::exception& e)
	{
		PrintException(&e, "AppInit()");
	}
	catch (...)
	{
		PrintException(NULL, "AppInit()");
	}

	if (!fRet)
	{
		threadGroup.interrupt_all();
		// threadGroup.join_all(); was left out intentionally here, because we didn't re-test all of
		// the startup-failure cases to make sure they don't result in a hang due to some
		// thread-blocking-waiting-for-another-thread-during-startup case
	}
	else
	{
		WaitForShutdown(&threadGroup);
	}

	Shutdown();

	return fRet;
}

int main(int argc, char* argv[])
{
	bool fRet = false;
	
	// Connect bitcoind signal handlers
	noui_connect();

	fRet = AppInit(argc, argv);

	if (fRet && fDaemon)
	{
		return 0;
	}

	return (fRet ? 0 : 1);
}

