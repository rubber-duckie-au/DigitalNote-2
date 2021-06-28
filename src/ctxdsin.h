#ifndef CTXDSIN_H
#define CTXDSIN_H

#include "ctxin.h"

/** Holds an MNengine input
 */
class CTxDSIn : public CTxIn
{
public:
	bool fHasSig;								// flag to indicate if signed
	int nSentTimes;								//times we've sent this anonymously

	CTxDSIn(const CTxIn& in);
};

#endif // CTXDSIN_H
