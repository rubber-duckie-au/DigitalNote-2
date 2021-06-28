#include "csemaphoregrant.h"

CSemaphoreGrant::CSemaphoreGrant() : sem(NULL), fHaveGrant(false)
{
	
}

CSemaphoreGrant::CSemaphoreGrant(CSemaphore &sema, bool fTry) : sem(&sema), fHaveGrant(false)
{
	if (fTry)
	{
		TryAcquire();
	}
	else
	{
		Acquire();
	}
}

CSemaphoreGrant::~CSemaphoreGrant()
{
	Release();
}

CSemaphoreGrant::operator bool()
{
	return fHaveGrant;
}

void CSemaphoreGrant::Acquire()
{
	if (fHaveGrant)
	{
		return;
	}
	
	sem->wait();
	fHaveGrant = true;
}

void CSemaphoreGrant::Release()
{
	if (!fHaveGrant)
	{
		return;
	}
	
	sem->post();
	fHaveGrant = false;
}

bool CSemaphoreGrant::TryAcquire()
{
	if (!fHaveGrant && sem->try_wait())
	{
		fHaveGrant = true;
	}
	
	return fHaveGrant;
}

void CSemaphoreGrant::MoveTo(CSemaphoreGrant &grant)
{
	grant.Release();
	grant.sem = sem;
	grant.fHaveGrant = fHaveGrant;
	sem = NULL;
	fHaveGrant = false;
}

