#ifndef MASTERNODE_H
#define MASTERNODE_H

#include <climits>

class uint256;

#define MASTERNODE_NOT_PROCESSED		0 // initial state
#define MASTERNODE_IS_CAPABLE			1
#define MASTERNODE_NOT_CAPABLE			2
#define MASTERNODE_STOPPED				3
#define MASTERNODE_INPUT_TOO_NEW		4
#define MASTERNODE_PORT_NOT_OPEN		6
#define MASTERNODE_PORT_OPEN			7
#define MASTERNODE_SYNC_IN_PROCESS		8
#define MASTERNODE_REMOTELY_ENABLED		9

#define MASTERNODE_MIN_CONFIRMATIONS	7
#define MASTERNODE_MIN_DSEEP_SECONDS	(30*60)
// v2.0.0.9 FINDING-2026-002 (TODO 3.12b): dseep dedup cache tuning.
// Retention == the +/-1h dseep accept window, so an entry lives at least as long
// as a replay of it could still fall inside the window.  MAX is a memory backstop.
#define MASTERNODE_DSEEP_SEEN_RETENTION	(60*60)
#define MASTERNODE_DSEEP_SEEN_MAX		10000
#define MASTERNODE_MIN_DSEE_SECONDS		(5*60)
#define MASTERNODE_PING_SECONDS			(1*60)
#define MASTERNODE_EXPIRATION_SECONDS	(65*60)
#define MASTERNODE_REMOVAL_SECONDS		(70*60)

// ---------------------------------------------------------------------------
// v2.0.0.8 voted-payment consensus constants
//
// These govern the masternode-voted payment selection system introduced in
// v2.0.0.8.  Full design rationale lives in PhaseC-design.md; brief notes:
//
//   VOTED_CONSENSUS_ACTIVATION_HEIGHT
//     The MAINNET block height at which validators begin enforcing the
//     voted-consensus payment rule.  It is the mainnet floor read by
//     GetEffectiveVotedConsensusActivationHeight() (cblock.cpp); testnet has
//     its own floor (2000) there.
//
//     >>> v2.0.0.9 rc9 (Duckie, 2026-11-16): NOT SCHEDULED on mainnet. <<<
//     INT_MAX = never.  v2.0.0.9 nodes stay permissive and follow whatever
//     chain enforcing nodes build.  v2.0.1.0 sets the real height once the
//     enforcement model (V-plan Group 1) is finished and soaked.  rc1-rc8 of
//     v2.0.0.9 wired 1,480,000 here; that is withdrawn.  Mainnet also ignores
//     SPORK_15 (S1), so no spork can switch enforcement on in this build.
//
//     What "activation" changes, for reference:
//       - Before activation, and always on a v2.0.0.9 mainnet node: the
//         masternode-slot payee is NOT enforced by validators (the payment
//         amounts and the devops output still are); producers pick the
//         payee with FindOldestNotInVec.
//       - After activation: validators require the voted winner when their
//         vote tracker has one (strict check), else any payee they recognise
//         as a masternode (weak check), and always accept the devops address
//         in the masternode slot.  Producers with no voted winner defer, then
//         mint a devops-paid rescue block (producer policy, not a validity
//         rule; see ShouldMintRescueBlock).
//
//   VOTE_LOOKAHEAD
//     Length of each masternode's payee queue.  A masternode that sees block N
//     broadcasts ONE queue predicting the payees for N+1 .. N+VOTE_LOOKAHEAD.
//
//   VOTE_PAST_HORIZON
//     Queues for heights below (currentHeight - VOTE_PAST_HORIZON) are
//     rejected as too old.
//
//   VOTE_TIME_WINDOW_SECONDS
//     Maximum acceptable skew between a queue's nTimeSigned and the local
//     clock: +/-30 min.  (dsee/dseep/mnver allow +/-60 min; the two values
//     are independent -- do not "align" them.)
//
//   REORG_DEPTH_BUFFER
//     Reorg-stability margin.  It shifts candidate and voter eligibility
//     back from the target height (SimulateQueue, CountVotingEligible) and
//     bounds the queue-height acceptance window.  There is NO cache clearing
//     on deep reorgs; a disconnect erases only that height's queues.
//
//   MIN_ENABLED_FOR_CONSENSUS
//     Sanity floor.  If fewer masternodes than this are voting-eligible
//     (CountVotingEligible, not CountEnabled) for a height, no canonical
//     winner can form for it.
//
//   VOTED_CONSENSUS_THRESHOLD_NUMERATOR / VOTED_CONSENSUS_THRESHOLD_DENOMINATOR
//     Integer expression of the 60% threshold.  Implemented as
//     (votes * DENOM >= total * NUMER) to avoid floating-point.
//     3/5 == 60%.
//
//   MAX_EQUIVOCATIONS_PER_SESSION
//     NO EFFECT since the v2.0.0.8 hotfix (Issue 3): nothing marks
//     equivocators any more -- a second queue from the same voter at the same
//     height simply replaces the first if it is newer.  Kept only because the
//     dead equivocator code still references it (removal: v2.0.1.0).
//
//   MIN_VOTING_PROTOCOL_VERSION
//     The M1Q vote-queue capability floor: 62058.  Builds below it cannot
//     produce a vote queue.
//
//     >>> v2.0.0.9 (TODO 3.39): NO LONGER A CONSENSUS INPUT. <<<
//     CountVotingEligible() used to skip masternodes below this floor, using
//     the controller-stamped mn.protocolVersion -- gossip that two nodes can
//     hold differently, so it could split the denominator.  That filter is
//     gone; the denominator is collateral depth only.  This constant now only
//     labels peers and masternodes as queue-capable in getdeploymentstatus
//     (peers.consensus_capable, version_drift).  The notes below are kept for
//     history: they explain why it was never raised in lockstep.
//
//     HISTORICAL: this was temporarily lowered to 62057 for the debug-build
//     wedge-reproduction soak (so the test staker could form quorum with a
//     fleet then at 62057; see ledger S24 / S25).  That soak is over and the
//     value has been RETURNED TO 62058 -- the production rationale (the queue
//     denominator counts ONLY M1Q queue-capable MNs) is unchanged.
//
//     v2.0.0.9 / TODO 3.19 -- DO NOT raise this to track PROTOCOL_VERSION.
//     PROTOCOL_VERSION moved to 62059 for v2.0.0.9, but this floor stays at
//     62058 ON PURPOSE: v2.0.0.8 (62058) IS queue-capable.  When this floor
//     still gated the denominator, raising it to 62059 would have dropped
//     every v2.0.0.8 masternode out of it the instant v2.0.0.9 shipped -- the
//     below-floor dead chain TODO 3.16 exists to rescue, self-inflicted via a
//     constant.  It no longer gates consensus (see above), but it still defines
//     "queue-capable" in the reports, so 62058 remains the correct value.
// ---------------------------------------------------------------------------

#define VOTED_CONSENSUS_ACTIVATION_HEIGHT				INT_MAX		// NOT SCHEDULED (v2.0.0.9 rc9)
#define VOTE_LOOKAHEAD									10
#define VOTE_PAST_HORIZON								10
#define VOTE_TIME_WINDOW_SECONDS						(30 * 60)
#define REORG_DEPTH_BUFFER								10
#define MIN_ENABLED_FOR_CONSENSUS						5

// v2.0.0.9 W-17: slack when judging whether our peers are ahead of us.
// Mirrors the 144-block allowance net.cpp StartSync() uses for the same
// judgement.  CNode::nStartingHeight is fixed at connection time, so on
// long-lived connections it under-reports peers; the slack keeps a node that is
// merely a few blocks behind from being treated as un-synced.
#define PEER_HEIGHT_TOLERANCE							144

// v2.0.0.9 blindness guard: how far back to look for masternode payees we have
// never held an entry for.
//
// Wants to cover two or three full payment rotations, so that every live
// masternode has had a turn.  It does NOT have to be tuned against departed
// masternodes -- setEverKnownPayees already filters those out -- which is why a
// generous value is safe here.  Measure on testnet before treating 200 as final.
#define ROSTER_COMPLETENESS_WINDOW						200

// v2.0.0.9: how often a node may re-ask peers for the full masternode list
// while its roster still looks short.
//
// DsegUpdate() keeps its own 3-hour guard PER PEER, so a retry only actually
// reaches peers not yet asked -- which is the point: it picks up newly connected
// peers rather than nagging the same one.
#define ROSTER_LIST_RETRY_SECS							(2*60)

// v2.0.0.9: stall-triggered queue rebroadcast.
//
// >>> WHY THIS EXISTS. <<<  BroadcastQueue() fires from exactly ONE place --
// block connect (main.cpp).  That is correct for WHEN the data changes, since a
// queue is a deterministic forward simulation from the tip and the tip is its
// only input.  But it also made block connect the only DELIVERY opportunity,
// and those are different concerns.
//
// The result was circular: no block -> no queue -> no canonical winner -> every
// producer defers -> no block.  SyncQueues() cannot break it, because that path
// only RELAYS queues a peer already holds; after a fleet-wide restart nobody
// holds any.  The only exit was to wait out RESCUE_STALL_SECS and mint a
// devops-paid rescue block -- diverting a masternode payment to recover from a
// condition where every masternode was alive and able to produce a queue.
//
// Observed on testnet 2026-10-17: fleet restarted onto rc4 while the chain was
// already at a standstill; 8/8 masternodes enabled and voting-eligible, and
// "0/10 queue-heights had queues, 0 votes examined" for 15 minutes.
//
// So: if the chain has stalled and this masternode has no queue of its own
// recorded at the current tip, rebroadcast ONE out of band.  BroadcastQueue()
// GENERATES from the tip (GetQueuePaymentSnapshot + SimulateQueue), so it needs
// no prior queue -- which is exactly what SyncQueues could not offer.
//
// Deliberately shorter than RESCUE_STALL_SECS and longer than a normal block:
// the cheap, correct recovery is tried first, and the rescue stays as the
// backstop for a genuine below-floor failure that no rebroadcast can fix.
#define QUEUE_REBROADCAST_STALL_SECS					(8*60)

// Rate limit per node. A long stall must not become a broadcast loop. Peers
// already drop duplicates via AlreadyHaveQueue(), so this is about our own
// outbound cost, not theirs.
#define QUEUE_REBROADCAST_MIN_GAP_SECS					(2*60)

// v2.0.0.9: how often to recompute the roster-completeness flag.
// The scan walks mapHistoricalPayees under mnodeman.cs; blindness changes over
// minutes and only gates a 10-minute delay, so per-second was wasteful.
#define ROSTER_REFRESH_SECONDS							30

// v2.0.0.9 W-15 option B: targeted self-lookup ("dsegk").
//
// One answer per (peer IP, requested key) per window, and at most
// DSEGK_MAX_KEYS_PER_PEER distinct keys per IP per window so that scanning
// random keys stays bounded.  Generous compared with the full-list limit because
// the reply is one entry, not the whole roster.
#define DSEGK_ASK_AGAIN_SECONDS							(5*60)
#define DSEGK_MAX_KEYS_PER_PEER							64
#define DSEGK_MAX_REPLIES								10

// v2.0.0.9 blindness guard: how long a demonstrably-short roster DELAYS the
// rescue before it is allowed anyway.
//
// Long enough for a dseg round trip and the resulting gossip to land; short
// enough that it does not materially extend a genuine stall (30 -> 40 min).
//
// >>> IT MUST NOT RESET WHILE THE NODE IS STILL BLIND. <<<  A timer pushed
// forward by each newly-arriving entry would let a node receiving a slow trickle
// of gossip defer for ever, which is FINDING-2026-011 through the back door.
// One delay, once per blind episode.
#define ROSTER_BLIND_RESCUE_DELAY_SECS					(10*60)
#define VOTED_CONSENSUS_THRESHOLD_NUMERATOR				3
#define VOTED_CONSENSUS_THRESHOLD_DENOMINATOR			5
#define MAX_EQUIVOCATIONS_PER_SESSION					3
#define MIN_VOTING_PROTOCOL_VERSION						62058

// v2.0.0.8 M1Q -- queue-based voting.
//   VOTE_QUEUE_LENGTH
//     The number of forward payee positions each queue carries.  Position
//     p (0-indexed) predicts the payee for height (nQueueHeight + 1 + p).
//     Equal to VOTE_LOOKAHEAD so that any height is covered by up to
//     VOTE_LOOKAHEAD in-flight queues after warm-up.  See
//     v208-M1Q-queue-based-voting-SPEC.md S4.
//   VOTE_COMMIT_BUFFER
//     GetCanonicalWinnerFromQueues returns no winner until the chain has
//     reached (targetHeight - VOTE_COMMIT_BUFFER).  Gives late-arriving
//     queues time to propagate before the consensus read commits.  See
//     spec S9.  Reorgs shallower than this preserve in-flight commits;
//     deeper reorgs may disturb them (spec S10.2).
#define VOTE_QUEUE_LENGTH								VOTE_LOOKAHEAD
#define VOTE_COMMIT_BUFFER								3

// Maximum depth the initial chain-walk for mapLastPaidHeight will look back.
// At observed 3.23min/block, 50000 blocks is ~112 days of history -- enough
// to find a recent payment for every active MN.
#define MAX_LASTPAID_SCAN_DEPTH							50000

// ---------------------------------------------------------------------------
// VOTER_ELIGIBILITY_DEPTH
//   Minimum chain depth, relative to the block being voted on, that a
//   masternode's 2,000,000 XDN collateral must have before that masternode
//   counts toward the voted-consensus denominator.
//
//   = MASTERNODE_MIN_CONFIRMATIONS (collateral maturity, 7) +
//     REORG_DEPTH_BUFFER          (reorg-stability margin, 10)
//
//   The buffer keeps the collateral-depth part of eligibility for height N
//   stable under any reorg shallower than REORG_DEPTH_BUFFER (see
//   VOTE_PAST_HORIZON and the queue-height window).  It does NOT make
//   CountVotingEligible(N) identical on every node: WHICH masternodes are in
//   the roster is gossip, and two nodes can hold different rosters (see
//   CountVotingEligible and the roster-completeness notes in
//   cmasternodeman.cpp).  GetCanonicalWinnerFromQueues uses this count as its
//   denominator.
// ---------------------------------------------------------------------------
#define VOTER_ELIGIBILITY_DEPTH							(MASTERNODE_MIN_CONFIRMATIONS + REORG_DEPTH_BUFFER)

bool GetBlockHash(uint256& hash, int nBlockHeight);

#endif // MASTERNODE_H
