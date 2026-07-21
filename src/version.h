#ifndef VERSION_H
#define VERSION_H

#include "clientversion.h"
#include <stdint.h>
#include <string>

//
// client versioning
//

static const int CLIENT_VERSION = 1000000 * CLIENT_VERSION_MAJOR
								+ 10000 * CLIENT_VERSION_MINOR
								+ 100 * CLIENT_VERSION_REVISION
								+ 1 * CLIENT_VERSION_BUILD;

extern const std::string CLIENT_NAME;
extern const std::string CLIENT_BUILD;
extern const std::string CLIENT_DATE;

//
// database format versioning
//
static const int DATABASE_VERSION = 70509;

//
// network protocol versioning
//
// Lineage (62057 and 62058 are BOTH v2.0.0.8 -- distinguished pre- vs
// post-queue so nodes can tell M1Q-capable peers from the earlier build):
//   62055 = v2.0.0.7
//   62057 = v2.0.0.8 PRE-QUEUE  (per-height single vote -- the earlier
//           testnet build; this protocol is now superseded/dead)
//   62058 = v2.0.0.8 POST-QUEUE (M1Q queue-based voting).
//           Adds the mnvotequeue / getmnqueues messages; it is the version
//           at which a node is counted as a queue-voting peer.
//   62059 = v2.0.0.9 (this build).  CONSENSUS-CAPABLE: this is the first
//           build whose mainnet voted-consensus activation is actually
//           wired (FINDING-2026-003: the resolver now reads
//           VOTED_CONSENSUS_ACTIVATION_HEIGHT = 1480000; every earlier
//           mainnet build resolved to INT_MAX and could NEVER activate).
//           It also carries the 3.16 devops rescue rule.
//
// WHY 62059 EXISTS (TODO 3.19).  With activation set in stone at 1480000 the
// network holds two behaviourally distinct classes that are otherwise hard to
// tell apart: pre-2.0.0.9 builds (activation == INT_MAX, voted consensus can
// never fire) and 2.0.0.9 builds (activation == 1480000, it will).  A distinct
// protocol version makes that visible in the version handshake, so getpeerinfo
// and the deployment-status telemetry can COUNT consensus-capable peers ahead
// of the M8 proceed/defer decision at activation_height - 2 weeks, instead of
// inferring it from subver strings.
//
// ---------------------------------------------------------------------------
// DO NOT "BUMP THE OTHER TWO IN LOCKSTEP".  Three constants look related; only
// this one moves for v2.0.0.9.
//
//   MIN_PEER_PROTO_VERSION (62052) -- intentionally NOT bumped.  Raising it
//     would DISCONNECT pre-2.0.0.9 peers, partitioning the network during the
//     rollout.  2.0.0.9 must identify itself while still interoperating.
//     Fencing off old peers is a SEPARATE decision for the M8 window, taken on
//     the telemetry this bump enables -- not a side effect of it.
//
//   MIN_VOTING_PROTOCOL_VERSION (62058, masternode.h) -- intentionally NOT
//     bumped, and this is the dangerous one.  It is the protocol floor for the
//     voted-consensus DENOMINATOR: cmasternodevotetracker.cpp calls
//     CountVotingEligible(N, MIN_VOTING_PROTOCOL_VERSION) and then requires
//     eligibleVoters >= MIN_ENABLED_FOR_CONSENSUS (5).  Bumping it to 62059
//     would exclude every v2.0.0.8 masternode (62058) from the denominator the
//     moment 2.0.0.9 ships.  Until 5+ masternodes have upgraded, eligibleVoters
//     would fall below the floor and voted consensus would stop resolving --
//     i.e. the below-floor dead chain that TODO 3.16 exists to rescue, induced
//     deliberately by a version constant.  The v2.0.0.8 fleet is M1Q
//     queue-capable and must keep counting.  Raise it only after the fleet has
//     demonstrably migrated, as its own decision with its own soak.
// ---------------------------------------------------------------------------
static const int PROTOCOL_VERSION = 62059;

// intial proto version, to be increased after version/verack negotiation
static const int INIT_PROTO_VERSION = 209;

// disconnect from peers older than this proto version
static const int MIN_PEER_PROTO_VERSION = 62052;

// minimum peer version accepted by MNenginePool
static const int MIN_POOL_PEER_PROTO_VERSION = 62050;
static const int MIN_INSTANTX_PROTO_VERSION = 62050;

//! minimum peer version that can receive masternode payments
// V1 - Last protocol version before update
// V2 - Newest protocol version
static const int MIN_MASTERNODE_PAYMENT_PROTO_VERSION_1 = 62051;
static const int MIN_MASTERNODE_PAYMENT_PROTO_VERSION_2 = 62051;

// nTime field added to CAddress, starting with this version;
// if possible, avoid requesting addresses nodes older than this
static const int CADDR_TIME_VERSION = 31402;

// only request blocks from nodes outside this range of versions
static const int NOBLKS_VERSION_START = 0;
static const int NOBLKS_VERSION_END = 62051;

// hard cutoff time for legacy network connections
static const int64_t HRD_LEGACY_CUTOFF = 9993058800; // OFF (NOT TOGGLED)

// hard cutoff time for future network connections
static const int64_t HRD_FUTURE_CUTOFF = 9993058800; // OFF (NOT TOGGLED)

// BIP 0031, pong message, is enabled for all versions AFTER this one
static const int BIP0031_VERSION = 60000;

// "mempool" command, enhanced "getdata" behavior starts with this version:
static const int MEMPOOL_GD_VERSION = 60002;

#endif
