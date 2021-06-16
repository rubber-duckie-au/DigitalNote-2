#ifndef CCRITICALBLOCK_H
#define CCRITICALBLOCK_H

#include "cmutexlock.h"
#include "types/ccriticalsection.h"

typedef CMutexLock<CCriticalSection> CCriticalBlock;

#endif // CCRITICALBLOCK_H
