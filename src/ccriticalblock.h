#ifndef CCRITICALBLOCK_H
#define CCRITICALBLOCK_H

#include "cmutexlock.h"
#include "ccriticalsection.h"

typedef CMutexLock<CCriticalSection> CCriticalBlock;

#define LOCK(cs) CCriticalBlock criticalblock(cs, #cs, __FILE__, __LINE__)
#define LOCK2(cs1,cs2) CCriticalBlock criticalblock1(cs1, #cs1, __FILE__, __LINE__),criticalblock2(cs2, #cs2, __FILE__, __LINE__)
#define TRY_LOCK(cs,name) CCriticalBlock name(cs, #cs, __FILE__, __LINE__, true)

#define ENTER_CRITICAL_SECTION(cs) \
    { \
        EnterCritical(#cs, __FILE__, __LINE__, (void*)(&cs)); \
        (cs).lock(); \
    }

#define LEAVE_CRITICAL_SECTION(cs) \
    { \
        (cs).unlock(); \
        LeaveCritical(); \
    }

#endif // CCRITICALBLOCK_H
