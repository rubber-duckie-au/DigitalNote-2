#ifndef TXPAIR_H
#define TXPAIR_H

#include <utility>

class CWalletTx;
class CAccountingEntry;

typedef std::pair<CWalletTx*, CAccountingEntry*> TxPair;

#endif // TXPAIR_H
