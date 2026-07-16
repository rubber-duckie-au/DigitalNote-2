#ifndef VELOCITY_H
#define VELOCITY_H

#include "fork.h"
#include "mining.h"
#include "main_const.h"

class CBlock;
class CBlockIndex;

static const int VELOCITY_HEIGHT[] = { VELOCITY_TOGGLE };		// Height to start Velocity
static const int VELOCITY_TERMINAL[] = { VELOCITY_TDIFF };		// Height to start Velocity retargetting
static const int VELOCITY_MAX_RATE[] = { BLOCK_SPACING_MAX };	// Rate to Velocity in seconds
static const int VELOCITY_RATE[] = { BLOCK_SPACING };			// Rate to Velocity in seconds
static const int VELOCITY_MIN_RATE[] = { BLOCK_SPACING_MIN };	// Rate to Velocity in seconds
static const unsigned int VELOCITY_MIN_TX[] = { MIN_TX_COUNT };	// Minimum amount (not value of!) of TX in a block to bypass Velocity-Rate
static const int VELOCITY_MIN_VALUE[] = { MIN_TX_VALUE };		// Minimum value of the TX in a block to bypass Velocity-Rate (without COIN base)
static const int VELOCITY_MIN_FEE[] = { MIN_TX_FEE };			// Minimum value of accumulated fees of the TX in a block to bypass Velocity-Rate (without COIN base)
static const bool VELOCITY_FACTOR[] = { false };				// Treat Switches as factors of BlockReward
static const bool VELOCITY_EXPLICIT[] = { false };				// Require all switches to trigger a block

bool Velocity_check(int nHeight);

// HOTFIX (v2.0.0.8.1): Velocity() now returns a 3-state result so the
// caller can distinguish ban-worthy rejections (suspicious behaviour)
// from benign rejections (clock skew, transient noise).  Previously
// the caller responded to every Velocity rejection with DoS(100),
// which caused mass false-positive bans of honest peers during the
// volatile bootstrap window when nTimeOffset is still settling.
enum class VelocityResult
{
	Accepted,
	RejectedBenign,    // clock skew or other benign condition; peer not at fault
	RejectedSevere     // attack-like (too-fast spacing, etc.); DoS-worthy
};

VelocityResult Velocity(CBlockIndex* prevBlock, CBlock* block);

int VelocityI(int nHeight);

#endif // VELOCITY_H
