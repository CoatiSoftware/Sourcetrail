
#include "template.h"

template <typename T>
T sum(T a, T b)
{
	return a + b;
}

template <>
int sum<int>(int a, int b)
{
	return a + b;
}

void work()
{
	double x = sum<double>(0.1, 0.2);
	int y = sum<int>(-1, 2);
	unsigned int z = sum<unsigned int>(1, 2);

	int v = diff<int>(9, 5);
	double w = diff<double>(0.5, 0.3);
}
