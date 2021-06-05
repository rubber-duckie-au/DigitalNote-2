#ifndef VECTOR_UCHAR_H
#define VECTOR_UCHAR_H

#include <vector>

#include "allocators/zero_after_free_allocator.h"

typedef std::vector<unsigned char, zero_after_free_allocator<unsigned char> > vector_uchar;

#endif // VECTOR_UCHAR_H
