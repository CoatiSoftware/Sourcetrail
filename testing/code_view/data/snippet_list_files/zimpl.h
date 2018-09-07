
#include "header.h"

int multiply(int a, int b)
{
	int r = 0;
	for (int i = 1; i < b; i++)
	{
		r = sum(r, a);
	}
	return r;
}
