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
//   62059 = v2.0.0.9 (this build).  Mainnet voted-consensus activation is
//           NOT SCHEDULED (VOTED_CONSENSUS_ACTIVATION_HEIGHT = INT_MAX, and
//           mainnet ignores SPORK_15) -- the same effective mainnet behaviour
//           as every earlier build.  rc1-rc8 of v2.0.0.9 wired 1,480,000
//           (FINDING-2026-003); rc9 withdrew that (Duckie, 2026-11-16).  It also
//           carries the producer-side devops rescue policy (TODO 3.16), which
//           is not a validity rule.
//
// WHY 62059 EXISTS (TODO 3.19).  It identifies v2.0.0.9 nodes in the version
// handshake, so getpeerinfo and the deployment-status telemetry can count
// upgraded peers instead of inferring it from subver strings.  It was bumped
// when activation was wired to 1,480,000 so the network could tell
// activating builds from non-activating ones; with activation not scheduled
// in rc9 it remains an adoption marker for the release that v2.0.1.0's
// activation will build on.
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
//     bumped.  It is the M1Q vote-queue capability floor, and v2.0.0.8
//     (62058) is queue-capable.  It used to gate the voted-consensus
//     DENOMINATOR, where bumping it to 62059 would have excluded every v2.0.0.8
//     masternode the moment 2.0.0.9 shipped and stalled voted consensus -- the
//     below-floor dead chain TODO 3.16 exists to rescue.  Since v2.0.0.9
//     (TODO 3.39) CountVotingEligible() applies no version filter at all, so
//     this constant only labels peers and masternodes as queue-capable in
//     getdeploymentstatus.  62058 is still the right value for that.
// ---------------------------------------------------------------------------
static const int PROTOCOL_VERSION = 62059;

// intial proto version, to be increased after version/verack negotiation
static const int INIT_PROTO_VERSION = 209;

// disconnect from peers older than this proto version
static const int MIN_PEER_PROTO_VERSION = 62052;

// minimum peer version accepted by MNenginePool
static const int MIN_POOL_PEER_PROTO_VERSION = 62050;
static const int MIN_INSTANTX_PROTO_VERSION = 62050;

// Legacy (inherited).  Despite the old name, NO payee path filters on these:
// they only set the default protocol floor for CountEnabled() (display) via
// CMasternodePayments::GetMinMasternodePaymentsProto().  Both are 62051, so
// SPORK_10 (which picks between them) changes nothing.
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
