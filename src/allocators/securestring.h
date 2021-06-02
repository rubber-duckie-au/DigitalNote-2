#ifndef SECURESTRING_H
#define SECURESTRING_H

#include <string>

#include "allocators/secure_allocator.h"

// This is exactly like std::string, but with a custom allocator.
typedef std::basic_string<char, std::char_traits<char>, secure_allocator<char>> SecureString;

#endif // SECURESTRING_H
