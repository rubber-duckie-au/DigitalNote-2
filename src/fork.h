// Copyright (c) 2016-2020 The CryptoCoderz Team / Espers
// Copyright (c) 2018-2020 The Rubix project
// Copyright (c) 2018-2020 The DigitalNote project
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_FORK_H
#define BITCOIN_FORK_H

#include <cstdint>
#include <string>
#include <map>

/** Reserve Phase start block */ 
static const int64_t nReservePhaseStart = 1;
/** Masternode/Devops Payment Update 1 **/
static const int64_t nPaymentUpdate_1 = 1558310400;
/** Masternode/Devops Payment Update 2 **/
static const int64_t nPaymentUpdate_2 = 1562094000;
/** Masternode/Devops Payment Update 3 **/
static const int64_t nPaymentUpdate_3 = 1562281200;
/** Velocity toggle block */
static const int64_t VELOCITY_TOGGLE = 175; // Implementation of the Velocity system into the chain.
/** Velocity retarget toggle block */
static const int64_t VELOCITY_TDIFF = 0; // Use Velocity's retargetting method.
/** Protocol 3.0 toggle */

/**
	https://www.epochconverter.com/
*/
static std::map<std::string, int64_t> mapEpochUpdateName = {
	{ "PaymentUpdate_1", 1558310400 }, // Monday, 20 May 2019 00:00:00 GMT
	{ "PaymentUpdate_2", 1562094000 }, // Tuesday, 2 July 2019 19:00:00 GMT
	{ "PaymentUpdate_3", 1562281200 }, // Thursday, 4 July 2019 23:00:00 GMT
	{ "PaymentUpdate_4", 1631232000 }, // Friday, 10 September 2021 00:00:00 GMT
};

#endif // BITCOIN_FORK_H
