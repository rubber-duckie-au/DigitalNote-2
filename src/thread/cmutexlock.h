#ifndef CMUTEXLOCK_H
#define CMUTEXLOCK_H

#include <boost/thread/locks.hpp>

/** Wrapper around boost::unique_lock<Mutex> */
template<typename Mutex>
class CMutexLock
{
private:
	boost::unique_lock<Mutex> lock;

public:
	CMutexLock(Mutex& mutexIn, const char* pszName, const char* pszFile, int nLine, bool fTry = false);
	~CMutexLock();

	operator bool();

private:
	void Enter(const char* pszName, const char* pszFile, int nLine);
	bool TryEnter(const char* pszName, const char* pszFile, int nLine);
};

#endif // CMUTEXLOCK_H
