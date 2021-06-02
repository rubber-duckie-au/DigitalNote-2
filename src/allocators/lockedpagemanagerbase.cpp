#include "allocators/memorypagelocker.h"

#include "allocators/lockedpagemanagerbase.h"

template <class Locker>
LockedPageManagerBase<Locker>::LockedPageManagerBase(size_t page_size) : page_size(page_size)
{
	// Determine bitmask for extracting page from address
	assert(!(page_size & (page_size-1))); // size must be power of two
	
	page_mask = ~(page_size - 1);
}


template <class Locker>
LockedPageManagerBase<Locker>::~LockedPageManagerBase()
{
	assert(this->GetLockedPageCount() == 0);
}

// For all pages in affected range, increase lock count
template <class Locker>
void LockedPageManagerBase<Locker>::LockRange(void *p, size_t size)
{
	boost::mutex::scoped_lock lock(mutex);
	
	if(!size)
	{
		return;
	}
	
	const size_t base_addr = reinterpret_cast<size_t>(p);
	const size_t start_page = base_addr & page_mask;
	const size_t end_page = (base_addr + size - 1) & page_mask;
	
	for(size_t page = start_page; page <= end_page; page += page_size)
	{
		Histogram::iterator it = histogram.find(page);
		
		if(it == histogram.end()) // Newly locked page
		{
			locker.Lock(reinterpret_cast<void*>(page), page_size);
			histogram.insert(std::make_pair(page, 1));
		}
		else // Page was already locked; increase counter
		{
			it->second += 1;
		}
	}
}

// For all pages in affected range, decrease lock count
template <class Locker>
void LockedPageManagerBase<Locker>::UnlockRange(void *p, size_t size)
{
	boost::mutex::scoped_lock lock(mutex);
	
	if(!size)
	{
		return;
	}
	
	const size_t base_addr = reinterpret_cast<size_t>(p);
	const size_t start_page = base_addr & page_mask;
	const size_t end_page = (base_addr + size - 1) & page_mask;
	
	for(size_t page = start_page; page <= end_page; page += page_size)
	{
		Histogram::iterator it = histogram.find(page);
		
		assert(it != histogram.end()); // Cannot unlock an area that was not locked
		
		// Decrease counter for page, when it is zero, the page will be unlocked
		it->second -= 1;
		
		if(it->second == 0) // Nothing on the page anymore that keeps it locked
		{
			// Unlock page and remove the count from histogram
			locker.Unlock(reinterpret_cast<void*>(page), page_size);
			
			histogram.erase(it);
		}
	}
}

// Get number of locked pages for diagnostics
template <class Locker>
int LockedPageManagerBase<Locker>::GetLockedPageCount()
{
	boost::mutex::scoped_lock lock(mutex);
	
	return histogram.size();
}

template class LockedPageManagerBase<MemoryPageLocker>;

