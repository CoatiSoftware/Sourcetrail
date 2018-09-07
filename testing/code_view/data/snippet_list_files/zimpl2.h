
#include "header.h"

int divide(int a, int b)
{
	int c = 0;
	int r = 0;
	while (r < a)
	{
		r = sum(r, b);
		c++;
	}
	return c;
}
