#include "caddress.h"
#include "cinv.h"

#include "mruset.h"

template <typename T>
mruset<T>::mruset(mruset<T>::size_type nMaxSizeIn)
{
	nMaxSize = nMaxSizeIn;
}

template <typename T>
typename mruset<T>::iterator mruset<T>::begin() const
{
	return set.begin();
}

template <typename T>
typename mruset<T>::iterator mruset<T>::end() const
{
	return set.end();
}

template <typename T>
typename mruset<T>::size_type mruset<T>::size() const
{
	return set.size();
}

template <typename T>
bool mruset<T>::empty() const
{
	return set.empty();
}

template <typename T>
typename mruset<T>::iterator mruset<T>::find(const key_type& k) const
{
	return set.find(k);
}

template <typename T>
typename mruset<T>::size_type mruset<T>::count(const key_type& k) const
{
	return set.count(k);
}

template <typename T>
void mruset<T>::clear()
{
	set.clear();
	queue.clear();
}

template <typename T>
typename std::pair<typename mruset<T>::iterator, bool> mruset<T>::insert(const key_type& x)
{
	std::pair<iterator, bool> ret = set.insert(x);
	
	if (ret.second)
	{
		if (nMaxSize && queue.size() == nMaxSize)
		{
			set.erase(queue.front());
			queue.pop_front();
		}
		
		queue.push_back(x);
	}
	
	return ret;
}

template <typename T>
typename mruset<T>::size_type mruset<T>::max_size() const
{
	return nMaxSize;
}

template <typename T>
typename mruset<T>::size_type mruset<T>::max_size(mruset<T>::size_type s)
{
	if (s)
	{
		while (queue.size() > s)
		{
			set.erase(queue.front());
			queue.pop_front();
		}
	}
	
	nMaxSize = s;
	
	return nMaxSize;
}

template class mruset<CAddress>;
template class mruset<CInv>;
