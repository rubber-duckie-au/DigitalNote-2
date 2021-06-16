#include "csemaphore.h"

CSemaphore::CSemaphore(int init) : value(init)
{
	
}

void CSemaphore::wait()
{
	boost::unique_lock<boost::mutex> lock(mutex);
	
	while (value < 1)
	{
		condition.wait(lock);
	}
	
	value--;
}

bool CSemaphore::try_wait()
{
	boost::unique_lock<boost::mutex> lock(mutex);
	
	if (value < 1)
	{
		return false;
	}
	
	value--;
	
	return true;
}

void CSemaphore::post()
{
	{
		boost::unique_lock<boost::mutex> lock(mutex);
		
		value++;
	}
	
	condition.notify_one();
}

