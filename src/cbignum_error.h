#ifndef CBIGNUM_ERROR_H
#define CBIGNUM_ERROR_H

#include <string>
#include <stdexcept>

/** Errors thrown by the bignum class */
class CBigNum_Error : public std::runtime_error
{
public:
	explicit CBigNum_Error(const std::string& str) : std::runtime_error(str)
	{
		
	}
};

#endif // CBIGNUM_ERROR_H
