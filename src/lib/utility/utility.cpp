#include "utility.h"

size_t utility::digits(size_t n)
{
	int digits = 1;

	while (n >= 10)
	{
		n /= 10;
		digits++;
	}

	return digits;
}
