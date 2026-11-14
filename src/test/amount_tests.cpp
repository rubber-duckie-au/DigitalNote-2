// Copyright (c) 2024-2025 DigitalNote XDN developers
// Distributed under the MIT software license.
// SPDX-License-Identifier: MIT
//
// src/test/amount_tests.cpp
//
// Tests for CAmount, monetary unit limits, and fee arithmetic.
// Uses real DigitalNote-2 constants from amount.h / main.h.
// Compiled into test_digitalnote alongside existing tests.

#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <limits>

#include "main_const.h"

// v2.0.0.9 (TODO 12.A.2 step 3): this fork has no MAX_SINGLE_TX.  The value that
// MoneyRange() validates against is MAX_SINGLE_TX (main_const.h:42,
// 10000000000 * COIN).  The upstream name was inherited by these tests and
// never existed here, so the cap was NEVER actually being asserted.
#include "types/camount.h"        // CAmount, COIN, CENT
#include "util.h"
// main.h for MIN_TX_FEE, MIN_RELAY_TX_FEE, MAX_SINGLE_TX - include guard-safe
#include "ctxout.h"        // v2.0.0.9 FINDING-2026-016: CTxOut::IsDust
#include "script.h"        // CScript, OP_* -- flat in this fork, not script/script.h
#include "main.h"

BOOST_AUTO_TEST_SUITE(AmountTests)

// -- Monetary constants --------------------------------------------------------

BOOST_AUTO_TEST_CASE(CoinEquals100000000Satoshis)
{
    BOOST_CHECK_EQUAL(COIN, CAmount(100000000));
}

BOOST_AUTO_TEST_CASE(CentEquals1000000Satoshis)
{
    BOOST_CHECK_EQUAL(CENT, CAmount(1000000));
}

BOOST_AUTO_TEST_CASE(MaxSingleTxIsPositive)
{
    BOOST_CHECK_GT(MAX_SINGLE_TX, CAmount(0));
}

BOOST_AUTO_TEST_CASE(MaxSingleTxFitsInInt64)
{
    BOOST_CHECK_LE(MAX_SINGLE_TX,
                   static_cast<CAmount>(std::numeric_limits<int64_t>::max()));
}

BOOST_AUTO_TEST_CASE(MoneyRangeAcceptsZero)
{
    BOOST_CHECK(MoneyRange(CAmount(0)));
}

BOOST_AUTO_TEST_CASE(MoneyRangeAcceptsMaxMoney)
{
    BOOST_CHECK(MoneyRange(MAX_SINGLE_TX));
}

BOOST_AUTO_TEST_CASE(MoneyRangeRejectsNegative)
{
    BOOST_CHECK(!MoneyRange(CAmount(-1)));
    BOOST_CHECK(!MoneyRange(CAmount(-COIN)));
}

BOOST_AUTO_TEST_CASE(MoneyRangeRejectsOverMax)
{
    BOOST_CHECK(!MoneyRange(MAX_SINGLE_TX + 1));
}

// -- Fee constants -------------------------------------------------------------

BOOST_AUTO_TEST_CASE(MinTxFeeIsPositive)
{
    BOOST_CHECK_GT(MIN_TX_FEE, CAmount(0));
}

BOOST_AUTO_TEST_CASE(MinRelayTxFeeIsPositive)
{
    BOOST_CHECK_GT(MIN_RELAY_TX_FEE, CAmount(0));
}

BOOST_AUTO_TEST_CASE(MinRelayTxFeeNotGreaterThanMinTxFee)
{
    // Relay fee should be <= min transaction fee
    BOOST_CHECK_LE(MIN_RELAY_TX_FEE, MIN_TX_FEE);
}

BOOST_AUTO_TEST_CASE(MinTxFeeUnderMaxMoney)
{
    BOOST_CHECK_LT(MIN_TX_FEE, MAX_SINGLE_TX);
}

// -- Arithmetic safety ---------------------------------------------------------

BOOST_AUTO_TEST_CASE(CoinMultiplicationDoesNotOverflow)
{
    // 21 million XDN * COIN should not exceed int64 max
    // DigitalNote total supply is well under 21M, but check the arithmetic
    CAmount supply = CAmount(21000000) * COIN;
    BOOST_CHECK_GT(supply, CAmount(0));
    BOOST_CHECK(MoneyRange(supply));
}

BOOST_AUTO_TEST_CASE(SmallAmountsAddCorrectly)
{
    CAmount a = CENT;
    CAmount b = CENT * 2;
    BOOST_CHECK_EQUAL(a + b, CAmount(3) * CENT);
}

BOOST_AUTO_TEST_CASE(AmountSubtractionIsCorrect)
{
    CAmount a = COIN;
    CAmount b = CENT;
    BOOST_CHECK_EQUAL(a - b, CAmount(99) * CENT);
}

// =====================================================================
// FINDING-2026-016 -- CTxOut::IsDust overflow.
//
// IsDust used to compute (nValue * 1000) FIRST, which overflows signed
// 64-bit above (2^63-1)/1000 = 9,223,372,036,854,775 units = 92,233,720
// XDN.  The product wrapped NEGATIVE, the output was declared dust, and
// CreateTransaction failed with "Transaction amount too small" -- for the
// LARGEST sends the wallet can be asked to make.  Reported on rc3,
// 2026-11-05.
//
// Upstream Bitcoin cannot reach this (21M supply at 1e8 units).  A single
// XDN testnet reserve block pays 80,000,000 XDN.
//
// These pin BOTH ends: the dust boundary behaves as it always did, AND
// large values are no longer misclassified.
//
// NOTE: these live here rather than in transaction_tests.cpp because that
// suite is on the CI KNOWN_BAD list -- it still carries upstream
// primitives/ include paths that do not exist in this fork, so it is not
// compiled and a test added there would never run.
// =====================================================================

static CTxOut DustTestOut(int64_t nValue)
{
	CTxOut out;

	out.nValue = nValue;
	// Plain 25-byte P2PKH scriptPubKey, so GetSerializeSize matches the
	// 34-byte txout the dust formula is written around.
	out.scriptPubKey = CScript() << OP_DUP << OP_HASH160
		<< std::vector<unsigned char>(20, 0x01) << OP_EQUALVERIFY << OP_CHECKSIG;

	return out;
}

BOOST_AUTO_TEST_CASE(IsDustSmallValuesAreStillDust)
{
	BOOST_CHECK(DustTestOut(0).IsDust(MIN_RELAY_TX_FEE));
	BOOST_CHECK(DustTestOut(1).IsDust(MIN_RELAY_TX_FEE));
}

BOOST_AUTO_TEST_CASE(IsDustBoundaryIsMonotonic)
{
	// Whatever the exact threshold, it must be ONE clean step: never dust
	// above a value that was already not dust.
	bool fSeenNonDust = false;

	for (int64_t v = 0; v <= 20000; v++)
	{
		const bool fDust = DustTestOut(v).IsDust(MIN_RELAY_TX_FEE);

		if (!fDust)
		{
			fSeenNonDust = true;
		}

		BOOST_CHECK(!(fDust && fSeenNonDust));
	}

	BOOST_CHECK(fSeenNonDust);
}

BOOST_AUTO_TEST_CASE(IsDustDoesNotOverflowOnLargeValues)
{
	// >>> THE REGRESSION. <<<  Every one of these was reported as dust by
	// the original expression, because (nValue * 1000) wrapped negative.
	BOOST_CHECK(!DustTestOut( 92233720LL * COIN).IsDust(MIN_RELAY_TX_FEE)); // just below
	BOOST_CHECK(!DustTestOut(100000000LL * COIN).IsDust(MIN_RELAY_TX_FEE)); // just above
	BOOST_CHECK(!DustTestOut(500000000LL * COIN).IsDust(MIN_RELAY_TX_FEE));
	BOOST_CHECK(!DustTestOut(MAX_SINGLE_TX).IsDust(MIN_RELAY_TX_FEE));
}

BOOST_AUTO_TEST_SUITE_END()
