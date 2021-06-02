#include "allocators/lockedpagemanager.h"
#include "support/cleanse.h"

#include "allocators/zero_after_free_allocator.h"

template<typename T>
zero_after_free_allocator<T>::zero_after_free_allocator() throw()
{
	
}

/*
template<typename T>
zero_after_free_allocator<T>::zero_after_free_allocator(const zero_after_free_allocator& a) throw() : base(a)
{
	
}

template<typename T>
template <typename U>
zero_after_free_allocator<T>::zero_after_free_allocator<U>(const zero_after_free_allocator<U>& a) throw() : base(a)
{
	
}
*/

template<typename T>
zero_after_free_allocator<T>::~zero_after_free_allocator() throw()
{
	
}

template<typename T>
void zero_after_free_allocator<T>::deallocate(T* p, std::size_t n)
{
	if (p != NULL)
	{
		memory_cleanse(p, sizeof(T) * n);
	}
	
	std::allocator<T>::deallocate(p, n);
}

template struct zero_after_free_allocator<unsigned char>;
template struct zero_after_free_allocator<char>;