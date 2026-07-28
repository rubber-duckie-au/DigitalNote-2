// Copyright (c) 2024-2026 DigitalNote XDN developers
// Distributed under the MIT software license.
// SPDX-License-Identifier: MIT
//
// src/test/spork_tests.cpp
//
// Spork IDs and the consensus-critical constants that must not drift.
//
// REWRITTEN 2026-07-28 (TODO 12.A.2 step 3).  The previous revision was
// inherited from upstream Bitcoin/Dash scaffolding and asserted things that
// are FALSE about this chain.  Because the test suite has never been
// executable (see test/CMakeLists.txt), none of it was ever caught:
//
//   * Spork IDs were asserted with a Dash-style 10000+N numbering
//     (SPORK_2 == 10002).  This fork numbers them 10000+(N-1), so
//     SPORK_2_INSTANTX is 10001.  EVERY id assertion was off by one.
//   * SPORK_9_MASTERNODE_SUPERBLOCKS does not exist.  Spork 9 here is
//     SPORK_9_MASTERNODE_BUDGET_ENFORCEMENT (10008); superblocks are
//     SPORK_13_ENABLE_SUPERBLOCKS (10012).
//   * MASTERNODE_MIN_CONFIRMATIONS was asserted >= 15.  It is 7.
//     That assertion would have FAILED.
//   * MN_COLLATERAL, nStakeMinDepth, nTargetSpacing, GetConsensus() and
//     CBaseChainParams::MAIN do not exist in this codebase at all.
//
// Assertions that were false or referenced non-existent symbols have been
// removed rather than "corrected until they compile".  What remains is
// derived from the tree and is intended as a drift guard on values that are
// wire-visible or consensus-bearing.

#include <boost/test/unit_test.hpp>
#include <vector>

#include "spork.h"			// SPORK_* ids
#include "masternode.h"		// MASTERNODE_MIN_CONFIRMATIONS, consensus gates
#include "version.h"		// PROTOCOL_VERSION, MIN_PEER_PROTO_VERSION
#include "kernel.h"			// STAKE_TIMESTAMP_MASK
#include "mining.h"			// BLOCK_SPACING
#include "fork.h"			// VERION_2_0_1_0_MANDATORY_UPDATE_BLOCK

BOOST_AUTO_TEST_SUITE(SporkTests)

// Spork IDs are wire values cross-checked between peers.  Never reuse or
// renumber them -- a collision or shift silently activates the wrong feature
// from stale broadcasts held in peers' mapSporksActive.

BOOST_AUTO_TEST_CASE(SporkIdsHaveExactExpectedValues)
{
	// Verified against spork.h 2026-07-28.  Numbering is 10000 + (N-1).
	BOOST_CHECK_EQUAL(SPORK_1_MASTERNODE_PAYMENTS_ENFORCEMENT, 10000);
	BOOST_CHECK_EQUAL(SPORK_2_INSTANTX,                        10001);
	BOOST_CHECK_EQUAL(SPORK_3_INSTANTX_BLOCK_FILTERING,        10002);
	BOOST_CHECK_EQUAL(SPORK_5_MAX_VALUE,                       10004);
	BOOST_CHECK_EQUAL(SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT,  10007);
	BOOST_CHECK_EQUAL(SPORK_9_MASTERNODE_BUDGET_ENFORCEMENT,   10008);
	BOOST_CHECK_EQUAL(SPORK_10_MASTERNODE_PAY_UPDATED_NODES,   10009);
	BOOST_CHECK_EQUAL(SPORK_13_ENABLE_SUPERBLOCKS,             10012);
	BOOST_CHECK_EQUAL(SPORK_14_TEST_SIGNATURES,                10013);
}

BOOST_AUTO_TEST_CASE(SporkIdsAreUnique)
{
	const std::vector<int> ids = {
		SPORK_1_MASTERNODE_PAYMENTS_ENFORCEMENT,
		SPORK_2_INSTANTX,
		SPORK_3_INSTANTX_BLOCK_FILTERING,
		SPORK_4_NOTUSED,
		SPORK_5_MAX_VALUE,
		SPORK_6_REPLAY_BLOCKS,
		SPORK_7_MASTERNODE_SCANNING,
		SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT,
		SPORK_9_MASTERNODE_BUDGET_ENFORCEMENT,
		SPORK_10_MASTERNODE_PAY_UPDATED_NODES,
		SPORK_11_RESET_BUDGET,
		SPORK_12_RECONSIDER_BLOCKS,
		SPORK_13_ENABLE_SUPERBLOCKS,
		SPORK_14_TEST_SIGNATURES,
	};

	for (size_t i = 0; i < ids.size(); ++i)
	{
		for (size_t j = i + 1; j < ids.size(); ++j)
		{
			BOOST_CHECK_NE(ids[i], ids[j]);
		}
	}
}

BOOST_AUTO_TEST_CASE(SporkIdsAreContiguousFrom10000)
{
	// A gap or duplicate here means an ID was renumbered -- exactly the
	// change the "Don't ever reuse these IDs" comment in spork.h forbids.
	BOOST_CHECK_EQUAL(SPORK_14_TEST_SIGNATURES
	                  - SPORK_1_MASTERNODE_PAYMENTS_ENFORCEMENT, 13);
}

BOOST_AUTO_TEST_SUITE_END()

// -----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ConsensusConstantTests)

// These are values a regression must never silently move.  Each is annotated
// with why it matters, so a failure explains itself.

BOOST_AUTO_TEST_CASE(MinVotingProtocolVersionIsStatic)
{
	// MUST remain 62058.  It gates the voted-consensus DENOMINATOR via
	// CountVotingEligible(N, MIN_VOTING_PROTOCOL_VERSION).  Raising it in
	// lockstep with PROTOCOL_VERSION would drop the entire v2.0.0.8
	// masternode fleet out of the denominator the instant a new build
	// shipped -- inducing the below-floor dead chain the devops rescue
	// exists to prevent.
	BOOST_CHECK_EQUAL(MIN_VOTING_PROTOCOL_VERSION, 62058);
}

BOOST_AUTO_TEST_CASE(MinPeerProtoVersionIsFrozen)
{
	// Frozen at 62052.  Raising it disconnects older peers.
	BOOST_CHECK_EQUAL(MIN_PEER_PROTO_VERSION, 62052);
}

BOOST_AUTO_TEST_CASE(ProtocolVersionIsAtLeastMinVoting)
{
	// PROTOCOL_VERSION moves when required; MIN_VOTING does not.  The former
	// must never fall below the latter.
	BOOST_CHECK_GE(PROTOCOL_VERSION, MIN_VOTING_PROTOCOL_VERSION);
}

BOOST_AUTO_TEST_CASE(VotedConsensusActivationHeightIsExact)
{
	// Must be exactly 1,480,000.  The Rust v3 port wires the same value; any
	// divergence splits the two implementations at that height.
	BOOST_CHECK_EQUAL(VOTED_CONSENSUS_ACTIVATION_HEIGHT, 1480000);
}

BOOST_AUTO_TEST_CASE(DevopsRotationHeightIsExact)
{
	// The v2.0.1.0 devops address rotation boundary.  Distinct from, and
	// earlier than, the voted-consensus activation height above.
	BOOST_CHECK_EQUAL(VERION_2_0_1_0_MANDATORY_UPDATE_BLOCK, 1280000);
	BOOST_CHECK_LT(VERION_2_0_1_0_MANDATORY_UPDATE_BLOCK,
	               VOTED_CONSENSUS_ACTIVATION_HEIGHT);
}

BOOST_AUTO_TEST_CASE(MinEnabledForConsensusIsFive)
{
	BOOST_CHECK_EQUAL(MIN_ENABLED_FOR_CONSENSUS, 5);
}

BOOST_AUTO_TEST_CASE(MasternodeMinConfirmationsIsSeven)
{
	// Verified value is 7.  The previous revision asserted >= 15 (an upstream
	// PoS-v3 assumption) and would have failed.
	BOOST_CHECK_EQUAL(MASTERNODE_MIN_CONFIRMATIONS, 7);
}

BOOST_AUTO_TEST_CASE(BlockSpacingIsTwoMinutes)
{
	BOOST_CHECK_EQUAL(BLOCK_SPACING, 120);
}

BOOST_AUTO_TEST_CASE(StakeTimestampMaskIsFifteen)
{
	// Stake timestamps are masked to 16-second granularity.  The Velocity
	// spacing logic depends on this value (FINDING-2026-010).
	BOOST_CHECK_EQUAL(STAKE_TIMESTAMP_MASK, 15);
}

BOOST_AUTO_TEST_SUITE_END()
