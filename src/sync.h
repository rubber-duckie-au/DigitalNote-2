// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_SYNC_H
#define BITCOIN_SYNC_H

#include "threadsafety.h"

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>


////////////////////////////////////////////////
//                                            //
// THE SIMPLE DEFINITON, EXCLUDING DEBUG CODE //
//                                            //
////////////////////////////////////////////////

/*
 
 
 
CCriticalSection mutex;
    boost::recursive_mutex mutex;

LOCK(mutex);
    boost::unique_lock<boost::recursive_mutex> criticalblock(mutex);

LOCK2(mutex1, mutex2);
    boost::unique_lock<boost::recursive_mutex> criticalblock1(mutex1);
    boost::unique_lock<boost::recursive_mutex> criticalblock2(mutex2);

TRY_LOCK(mutex, name);
    boost::unique_lock<boost::recursive_mutex> name(mutex, boost::try_to_lock_t);

ENTER_CRITICAL_SECTION(mutex); // no RAII
    mutex.lock();

LEAVE_CRITICAL_SECTION(mutex); // no RAII
    mutex.unlock();
 
 
 
 */



#ifdef DEBUG_LOCKORDER
void EnterCritical(const char* pszName, const char* pszFile, int nLine, void* cs, bool fTry = false);
void LeaveCritical();
std::string LocksHeld();
void AssertLockHeldInternal(const char* pszName, const char* pszFile, int nLine, void *cs);
#else
void static inline EnterCritical(const char* pszName, const char* pszFile, int nLine, void* cs, bool fTry = false) {}
void static inline LeaveCritical() {}
void static inline AssertLockHeldInternal(const char* pszName, const char* pszFile, int nLine, void *cs) {}
#endif
#define AssertLockHeld(cs) AssertLockHeldInternal(#cs, __FILE__, __LINE__, &cs)

#ifdef DEBUG_LOCKCONTENTION
void PrintLockContention(const char* pszName, const char* pszFile, int nLine);
#endif




class CSemaphore
{
private:
    boost::condition_variable condition;
    boost::mutex mutex;
    int value;

public:
    CSemaphore(int init) : value(init) {}

    void wait() {
        boost::unique_lock<boost::mutex> lock(mutex);
        while (value < 1) {
            condition.wait(lock);
        }
        value--;
    }

    bool try_wait() {
        boost::unique_lock<boost::mutex> lock(mutex);
        if (value < 1)
            return false;
        value--;
        return true;
    }

    void post() {
        {
            boost::unique_lock<boost::mutex> lock(mutex);
            value++;
        }
        condition.notify_one();
    }
};

/** RAII-style semaphore lock */
class CSemaphoreGrant
{
private:
    CSemaphore *sem;
    bool fHaveGrant;

public:
    void Acquire() {
        if (fHaveGrant)
            return;
        sem->wait();
        fHaveGrant = true;
    }

    void Release() {
        if (!fHaveGrant)
            return;
        sem->post();
        fHaveGrant = false;
    }

    bool TryAcquire() {
        if (!fHaveGrant && sem->try_wait())
            fHaveGrant = true;
        return fHaveGrant;
    }

    void MoveTo(CSemaphoreGrant &grant) {
        grant.Release();
        grant.sem = sem;
        grant.fHaveGrant = fHaveGrant;
        sem = NULL;
        fHaveGrant = false;
    }

    CSemaphoreGrant() : sem(NULL), fHaveGrant(false) {}

    CSemaphoreGrant(CSemaphore &sema, bool fTry = false) : sem(&sema), fHaveGrant(false) {
        if (fTry)
            TryAcquire();
        else
            Acquire();
    }

    ~CSemaphoreGrant() {
        Release();
    }

    operator bool() {
        return fHaveGrant;
    }
};
#endif

