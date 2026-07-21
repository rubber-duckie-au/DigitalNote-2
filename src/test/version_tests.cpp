// Copyright (c) 2024-2026 DigitalNote XDN developers
// Distributed under the MIT software license.
// SPDX-License-Identifier: MIT
//
// src/test/version_tests.cpp
//
// Tests for client version, protocol version, and the voting-protocol floor.
//
// Run:  ./src/test/test_digitalnote --run_test=VersionTests --log_level=all
//
// -----------------------------------------------------------------------------
// v2.0.0.9 REWRITE (2026-07-16) -- TODO 7.4 / 3.19 follow-up.
//
// The previous revision of this file was orphaned and had rotted badly:
//   * It asserted PROTOCOL_VERSION == 62055 and CLIENT_VERSION == 2000007 -- both
//     stale by two releases (actual: 62059 / 2000009).
//   * It referenced MIN_PROTO_VERSION and COPYRIGHT_YEAR, NEITHER OF WHICH EXISTS
//     anywhere in the tree.  So the file could not COMPILE, let alone pass -- which
//     is why it is not listed in include/app/sources.pri and had silently stopped
//     being built at some point before v2.0.0.7 shipped.
//   * It encoded brittle relative assertions ("exactly 62054 + 1", "gap of exactly
//     3 from MIN_PEER_PROTO_VERSION") guaranteed to break on every future bump
//     while testing nothing anyone relies on.
//
// This rewrite keeps the tests that earn their place and drops the rest:
//   * PINNING tests on values that must only ever change deliberately
//     (CLIENT_VERSION, PROTOCOL_VERSION, MIN_PEER_PROTO_VERSION,
//     MIN_VOTING_PROTOCOL_VERSION).  These exist to FAIL when someone bumps a
//     constant, forcing the bump to be conscious.
//   * INVARIANT tests on relationships that must hold at any values.
//   * A regression guard for the v2.0.0.9 MIN_VOTING_PROTOCOL_VERSION finding --
//     the highest-value test in this file.
//
// NOTE: this file is still NOT in sources.pri.  It is rewritten for consistency
// and so that it is correct if/when the test target is wired back up.  Wiring the
// test suite back into the build is its own decision (TODO Section 6).
// -----------------------------------------------------------------------------

#include <boost/test/unit_test.hpp>

#include "clientversion.h"
#include "version.h"
#include "masternode.h"
#include "util.h"

BOOST_AUTO_TEST_SUITE(VersionTests)

// -- CLIENT_VERSION constants -------------------------------------------------

BOOST_AUTO_TEST_CASE(ClientVersionComponentsAre_2_0_0_9)
{
	BOOST_CHECK_EQUAL(CLIENT_VERSION_MAJOR,    2);
	BOOST_CHECK_EQUAL(CLIENT_VERSION_MINOR,    0);
	BOOST_CHECK_EQUAL(CLIENT_VERSION_REVISION, 0);
	BOOST_CHECK_EQUAL(CLIENT_VERSION_BUILD,    9);
}

BOOST_AUTO_TEST_CASE(ClientVersionIsRelease)
{
	BOOST_CHECK(CLIENT_VERSION_IS_RELEASE);
}

BOOST_AUTO_TEST_CASE(ComputedClientVersionEquals2000009)
{
	// CLIENT_VERSION = 1000000*MAJOR + 10000*MINOR + 100*REVISION + 1*BUILD
	//                = 2000000 + 0 + 0 + 9 = 2000009
	BOOST_CHECK_EQUAL(CLIENT_VERSION, 2000009);
}

BOOST_AUTO_TEST_CASE(ComputedClientVersionMatchesItsComponents)
{
	// Invariant form -- holds at any version, catches component/composite drift.
	const int expected = 1000000 * CLIENT_VERSION_MAJOR
	                   +   10000 * CLIENT_VERSION_MINOR
	                   +     100 * CLIENT_VERSION_REVISION
	                   +       1 * CLIENT_VERSION_BUILD;

	BOOST_CHECK_EQUAL(CLIENT_VERSION, expected);
}

BOOST_AUTO_TEST_CASE(ClientVersionGreaterThanPreviousRelease)
{
	// Previous release was 2.0.0.8 -> integer 2000008.
	BOOST_CHECK_GT(CLIENT_VERSION, 2000008);
}

BOOST_AUTO_TEST_CASE(FormatFullVersionContains_2_0_0_9)
{
	std::string ver = FormatFullVersion();

	BOOST_CHECK(!ver.empty());
	BOOST_CHECK(ver.find("2.0.0.9") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(FormatSubVersionNotEmpty)
{
	std::vector<std::string> comments;
	std::string sub = FormatSubVersion("DigitalNote", CLIENT_VERSION, comments);

	BOOST_CHECK(!sub.empty());
	BOOST_CHECK_EQUAL(sub.front(), '/');
	BOOST_CHECK_EQUAL(sub.back(),  '/');
}

BOOST_AUTO_TEST_CASE(FormatSubVersionWithComments)
{
	std::vector<std::string> comments;
	comments.push_back("testnet");

	std::string sub = FormatSubVersion("DigitalNote", CLIENT_VERSION, comments);

	BOOST_CHECK(sub.find("testnet") != std::string::npos);
}

// -- PROTOCOL_VERSION ---------------------------------------------------------
//
// 5-digit Peercoin/Dash-lineage protocol version.  Lineage:
//   62055 = v2.0.0.7
//   62057 = v2.0.0.8 PRE-QUEUE  (superseded/dead)
//   62058 = v2.0.0.8 POST-QUEUE (M1Q queue-based voting)
//   62059 = v2.0.0.9            (consensus-capable: mainnet activation actually
//                                wired to 1480000; earlier builds were INT_MAX)

BOOST_AUTO_TEST_CASE(ProtocolVersionIs62059)
{
	// PINNED.  If this fails you bumped PROTOCOL_VERSION -- make sure that was
	// deliberate, then update this test AND the lineage comment in version.h.
	BOOST_CHECK_EQUAL(PROTOCOL_VERSION, 62059);
}

BOOST_AUTO_TEST_CASE(MinPeerProtoVersionIs62052)
{
	// PINNED and deliberately frozen.  Raising this DISCONNECTS older peers and
	// partitions the network during rollout.  Fencing off pre-2.0.0.9 peers is a
	// separate M8-window decision, never a side effect of a PROTOCOL_VERSION bump.
	BOOST_CHECK_EQUAL(MIN_PEER_PROTO_VERSION, 62052);
}

BOOST_AUTO_TEST_CASE(ProtocolVersionAboveMinPeerProto)
{
	// Invariant: we must advertise at least what we are willing to accept.
	BOOST_CHECK_GT(PROTOCOL_VERSION, MIN_PEER_PROTO_VERSION);
}

BOOST_AUTO_TEST_CASE(NewNodeAndPriorFleetInteroperate)
{
	// No partition either direction: a v2.0.0.9 node (62059) and the v2.0.0.8
	// fleet (62058) must each clear MIN_PEER_PROTO_VERSION.
	BOOST_CHECK_GE(62059, MIN_PEER_PROTO_VERSION);
	BOOST_CHECK_GE(62058, MIN_PEER_PROTO_VERSION);
}

// -- MIN_VOTING_PROTOCOL_VERSION (the consensus denominator floor) -------------
//
// *** THE MOST IMPORTANT TESTS IN THIS FILE. ***
//
// MIN_VOTING_PROTOCOL_VERSION gates the voted-consensus DENOMINATOR:
// cmasternodevotetracker.cpp calls
//     CountVotingEligible(N, MIN_VOTING_PROTOCOL_VERSION)
// and requires the result to be >= MIN_ENABLED_FOR_CONSENSUS (5).
// CountVotingEligible skips a masternode when mn.protocolVersion < the floor.
//
// masternode.h's historical rationale said the denominator should track
// PROTOCOL_VERSION "in lockstep".  Following that literally when PROTOCOL_VERSION
// moved to 62059 would have dropped EVERY v2.0.0.8 masternode (62058) out of the
// denominator the instant v2.0.0.9 shipped.  Until 5+ masternodes upgraded,
// eligibleVoters would sit below MIN_ENABLED_FOR_CONSENSUS and voted consensus
// would stop resolving -- the below-floor dead chain the 3.16 devops rescue exists
// to recover from, self-inflicted by a constant.  (Not hypothetical: rpcmining.cpp
// records CountVotingEligible transiently dropping to 4.)
//
// These tests exist so a future "tidy up the version constants in lockstep" change
// FAILS LOUDLY instead of stalling mainnet.

BOOST_AUTO_TEST_CASE(MinVotingProtocolVersionIs62058)
{
	// PINNED and deliberately STATIC across the v2.0.0.9 bump.  Raise it only
	// after the fleet has demonstrably migrated, as its own decision with its own
	// soak.  Read the guardrail comment in masternode.h before touching this.
	BOOST_CHECK_EQUAL(MIN_VOTING_PROTOCOL_VERSION, 62058);
}

BOOST_AUTO_TEST_CASE(MinVotingProtocolVersionDoesNotTrackProtocolVersion)
{
	// Regression guard for the v2.0.0.9 finding: the voting floor must NOT be
	// bumped in lockstep with PROTOCOL_VERSION.  If these ever become equal
	// because someone raised the floor to match a new PROTOCOL_VERSION, the
	// existing fleet is excluded from the consensus denominator.
	BOOST_CHECK_LT(MIN_VOTING_PROTOCOL_VERSION, PROTOCOL_VERSION);
}

BOOST_AUTO_TEST_CASE(PriorFleetStillCountsTowardConsensusDenominator)
{
	// The v2.0.0.8 fleet (62058) is M1Q queue-capable and MUST keep counting.
	// CountVotingEligible skips when mn.protocolVersion < MIN_VOTING_PROTOCOL_VERSION,
	// so this models the inclusion test directly.
	const int fleetProtocolVersion = 62058;

	BOOST_CHECK(!(fleetProtocolVersion < MIN_VOTING_PROTOCOL_VERSION));
}

BOOST_AUTO_TEST_CASE(NewBuildStillCountsTowardConsensusDenominator)
{
	// A v2.0.0.9 node (62059) must also count -- a bump must never exclude the
	// build doing the bumping.
	BOOST_CHECK(!(PROTOCOL_VERSION < MIN_VOTING_PROTOCOL_VERSION));
}

BOOST_AUTO_TEST_CASE(PreQueueBuildsExcludedFromConsensusDenominator)
{
	// The pre-queue builds are NOT queue-capable and must stay out of the
	// denominator: 62057 (v2.0.0.8 pre-queue) and 62055 (v2.0.0.7).
	BOOST_CHECK_LT(62057, MIN_VOTING_PROTOCOL_VERSION);
	BOOST_CHECK_LT(62055, MIN_VOTING_PROTOCOL_VERSION);
}

// -- Assorted protocol sanity --------------------------------------------------

BOOST_AUTO_TEST_CASE(CAddrTimeVersionSanity)
{
	BOOST_CHECK_EQUAL(CADDR_TIME_VERSION, 31402);
}

BOOST_AUTO_TEST_CASE(BIP0031VersionSanity)
{
	BOOST_CHECK_EQUAL(BIP0031_VERSION, 60000);
}

BOOST_AUTO_TEST_CASE(NoblksVersionRangeSane)
{
	BOOST_CHECK_LT(NOBLKS_VERSION_START, NOBLKS_VERSION_END);
}

BOOST_AUTO_TEST_CASE(InitProtoVersionBelowProtocolVersion)
{
	// INIT_PROTO_VERSION is the pre-negotiation version; it must be far below the
	// negotiated one.
	BOOST_CHECK_LT(INIT_PROTO_VERSION, PROTOCOL_VERSION);
}

BOOST_AUTO_TEST_SUITE_END()
