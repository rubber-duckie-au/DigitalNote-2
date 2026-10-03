#include "compat.h"

#include <algorithm>
#include <random>
#include <openssl/rand.h>

#include "init.h"
#include "util.h"
#include "instantx.h"
#include "cvalidationstate.h"
#include "cwallet.h"
#include "cwallettx.h"
#include "mining.h"
#include "creservekey.h"
#include "net/cnode.h"
#include "net.h"
#include "thread.h"   // v2.0.0.9 W-17: LOCK(cs_vNodes) for the list request
#include "chashwriter.h"
#include "ckey.h"
#include "cinv.h"
#include "main_extern.h"
#include "cblockindex.h"
#include "cactivemasternode.h"
#include "cmasternode.h"
#include "cmasternodeman.h"
#include "cmasternodepayments.h"
#include "masternode.h"
#include "masternodeman.h"
#include "masternode_extern.h"
#include "ctxdsin.h"
#include "ctxdsout.h"
#include "cmnengineentry.h"
#include "cmnenginequeue.h"
#include "cmnenginebroadcasttx.h"
#include "cmnenginesigner.h"
#include "cmnenginepool.h"
#include "mnengine_extern.h"

#include "mnengine.h"

// The main object for accessing mnengine
CMNenginePool mnEnginePool;
// A helper object for signing messages from Masternodes
CMNengineSigner mnEngineSigner;
// The current mnengines in progress on the network
std::vector<CMNengineQueue> vecMNengineQueue;
// keep track of the scanning errors I've seen
std::map<uint256, CMNengineBroadcastTx> mapMNengineBroadcastTxes;
// Keep track of the active Masternode
CActiveMasternode activeMasternode;

// count peers we've requested the list from
int RequestedMasterNodeList = 0;

//TODO: Rename/move to core
void ThreadCheckMNenginePool()
{
	if(fLiteMode)
	{
		return; //disable all MNengine/Masternode related functionality
	}

	// Make this thread recognisable as the wallet flushing thread
	RenameThread("DigitalNote-mnengine");

	unsigned int c = 0;

	while (true)
	{
		MilliSleep(1000);
		
		//LogPrintf("ThreadCheckMNenginePool::check timeout\n");

		// try to sync from all available nodes, one step at a time
		//masternodeSync.Process();
		
		// v2.0.0.9 W-17: peer-height refresh and the masternode-list request run
		// OUTSIDE the IsBlockchainSynced() gate below.
		//
		// >>> DO NOT MOVE THESE INSIDE IT. <<<  IsBlockchainSynced() only latches
		// true after it has seen a tip less than an hour old, so a node restarted
		// DURING A STALL never latches, and nothing inside that gate ever runs --
		// which is precisely when FINDING-2026-011 needs the masternode list in
		// order to break the stall.
		mnEnginePool.RefreshPeerHeightEstimate();
		// Throttled: this walks mapHistoricalPayees, which is bounded by ENTRIES
		// (MAX_LASTPAID_SCAN_DEPTH = 50000 distinct payee scripts), under
		// mnodeman.cs -- the same lock the dsee handler and payment selection need.
		// Running it every second would hold that lock needlessly often for a value
		// that changes over minutes, and which only gates a 10-minute delay.
		//
		// Timed off its OWN clock, not the loop counter c: c is only incremented
		// inside the IsBlockchainSynced() gate below, so on a syncing node -- or one
		// restarted during a stall, where that gate never latches -- c would stay 0,
		// "c % N == 0" would be true every iteration, and the throttle would not
		// exist in precisely the case this code was moved out of that gate to serve.
		static int64_t nLastRosterRefresh = 0;

		if(GetTime() - nLastRosterRefresh >= ROSTER_REFRESH_SECONDS)
		{
			nLastRosterRefresh = GetTime();

			mnodeman.RefreshRosterCompleteness();
		}

		// v2.0.0.9 W-15 option B: a cold masternode that is not yet enabled asks its
		// peers for ITS OWN entry, by operator key.
		//
		// A cold node reaches MASTERNODE_REMOTELY_ENABLED only by receiving its own
		// dsee.  Its previous route was the full-list dseg, rate limited per IP for 3
		// hours -- so on a host running several cold masternodes only the first to
		// restart got an answer and the others waited for an operator.
		//
		// OUTSIDE the IsBlockchainSynced() gate below, deliberately: ManageStatus()
		// sits inside it and does not run at all on a node restarted during a stall,
		// which is exactly when this is needed.
		//
		// Hot masternodes never come here -- they find their own collateral locally and
		// self-register to status 1.
		// >>> A HOT masternode must never ask. <<<  Its status is
		// MASTERNODE_IS_CAPABLE (1), which is != MASTERNODE_REMOTELY_ENABLED (9),
		// so testing only against 9 was true for hot nodes forever -- they asked
		// every 5 minutes indefinitely and could never reach a state that stopped
		// them.  Observed on testnet 2026-10-03: two hot masternodes on one host
		// requesting from 02:37 to 05:23 without pause.
		//
		// A hot masternode finds its own collateral locally and self-registers; it
		// has no use for its own dsee.  Only a node that is neither capable nor
		// enabled is genuinely waiting for one.
		if(fMasterNode &&
			activeMasternode.status != MASTERNODE_IS_CAPABLE &&
			activeMasternode.status != MASTERNODE_REMOTELY_ENABLED &&
			activeMasternode.pubKeyMasternode.IsValid() &&
			mnEnginePool.IsMasternodeListSyncable())
		{
			static int64_t nLastSelfLookup = 0;
		
			int64_t nNow = GetTime();
		
			if(nNow - nLastSelfLookup >= DSEGK_ASK_AGAIN_SECONDS)
			{
				nLastSelfLookup = nNow;
		
				LOCK(cs_vNodes);
		
				for(CNode* pnode : vNodes)
				{
					if(pnode == NULL || pnode->fDisconnect || !pnode->fSuccessfullyConnected)
					{
						continue;
					}
		
					pnode->PushMessage("dsegk", activeMasternode.pubKeyMasternode);
				}
		
				LogPrintf("ThreadCheckMNenginePool -- not enabled yet; asked peers for our own "
						  "masternode entry by operator key\n");
			}
		}
		
		// Ask ONE peer for the full list, once, after we are caught up.
		//
		// DsegUpdate has exactly one other call site (main.cpp, on connect), so a
		// node whose connections were all made while it was still behind -- or that
		// went blind after connecting -- never asked again and fell back to the slow
		// per-entry AskForMN path.  DsegUpdate keeps its own per-peer 3-hour guard,
		// so this cannot spam.
		// Ask peers for the full list -- once we are synced, and AGAIN while our roster
		// still looks short.
		//
		// The first version asked exactly once per process and set the flag when the
		// request was SENT, not when it was answered.  If that peer was an older build,
		// was rate limited, or replied partially, the node never asked again and limped
		// along on the slow per-entry AskForMN path.
		//
		// It also left the blindness guard in ShouldMintRescueBlock() with nothing to
		// wait FOR: that guard delays the rescue so a refresh can happen, and no
		// refresh was being attempted.  This closes that loop.
		//
		// DsegUpdate() self-guards per peer for 3 hours, so looping every peer is cheap
		// and naturally reaches only those not yet asked -- picking up newly connected
		// peers rather than nagging the same one.
		static int64_t nLastListRequest = 0;
		static bool    fAskedForListAfterSync = false;
		
		if (mnEnginePool.IsMasternodeListSyncable())
		{
			int64_t nNow = GetTime();
			bool fWantList = !fAskedForListAfterSync || mnodeman.IsRosterLikelyIncomplete();
		
			if (fWantList && (nNow - nLastListRequest) >= ROSTER_LIST_RETRY_SECS)
			{
				nLastListRequest = nNow;
		
				int nAsked = 0;
		
				{
					LOCK(cs_vNodes);
		
					for(CNode* pnode : vNodes)
					{
						if (pnode == NULL || pnode->fClient || pnode->fOneShot ||
							pnode->fDisconnect || !pnode->fSuccessfullyConnected)
						{
							continue;
						}
		
						mnodeman.DsegUpdate(pnode);
		
						nAsked++;
					}
				}
		
				if (nAsked > 0)
				{
					if (!fAskedForListAfterSync)
					{
						LogPrintf("ThreadCheckMNenginePool -- requesting masternode list from %d "
								  "peer(s) now that we are synced\n", nAsked);
					}
					else
					{
						LogPrintf("ThreadCheckMNenginePool -- roster still looks short; re-asking "
								  "%d peer(s) for the masternode list\n", nAsked);
					}
		
					fAskedForListAfterSync = true;
				}
			}
		}
		
		if(mnEnginePool.IsBlockchainSynced())
		{
			c++;

			// check if we should activate or ping every few minutes,
			// start right after sync is considered to be done
			if(c % MASTERNODE_PING_SECONDS == 1)
			{
				activeMasternode.ManageStatus();
			}
			
			if(c % 60 == 0)
			{
				mnodeman.CheckAndRemove();
				mnodeman.ProcessMasternodeConnections();
				masternodePayments.CleanPaymentList();
				CleanTransactionLocksList();
			}

			//if(c % MASTERNODES_DUMP_SECONDS == 0)
			//{
			//	DumpMasternodes();
			//}
			
			mnEnginePool.CheckTimeout();
			mnEnginePool.CheckForCompleteQueue();

			//if(mnEnginePool.GetState() == POOL_STATUS_IDLE && c % 15 == 0)
			//{
				
			//}
		}
	}
}

