#include "types/ccriticalsection.h"

#include "thread.h"

#include "cmutexlock.h"

template<typename Mutex>
CMutexLock<Mutex>::CMutexLock(Mutex& mutexIn, const char* pszName, const char* pszFile, int nLine, bool fTry)
		: lock(mutexIn, boost::defer_lock)
{
	if (fTry)
	{
		TryEnter(pszName, pszFile, nLine);
	}
	else
	{
		Enter(pszName, pszFile, nLine);
	}
}

template<typename Mutex>
CMutexLock<Mutex>::~CMutexLock()
{
	if (lock.owns_lock())
	{
		LeaveCritical();
	}
}

template<typename Mutex>
CMutexLock<Mutex>::operator bool()
{
	return lock.owns_lock();
}

template<typename Mutex>
void CMutexLock<Mutex>::Enter(const char* pszName, const char* pszFile, int nLine)
{
	EnterCritical(pszName, pszFile, nLine, (void*)(lock.mutex()));
	
#ifdef DEBUG_LOCKCONTENTION
	if (!lock.try_lock())
	{
		PrintLockContention(pszName, pszFile, nLine);
#endif

	lock.lock();

#ifdef DEBUG_LOCKCONTENTION
	}
#endif

}

template<typename Mutex>
bool CMutexLock<Mutex>::TryEnter(const char* pszName, const char* pszFile, int nLine)
{
	EnterCritical(pszName, pszFile, nLine, (void*)(lock.mutex()), true);
	
	lock.try_lock();
	
	if (!lock.owns_lock())
	{
		LeaveCritical();
	}
	
	return lock.owns_lock();
}

template class CMutexLock<CCriticalSection>;