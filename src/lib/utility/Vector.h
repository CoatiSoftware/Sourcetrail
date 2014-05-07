#ifndef VECTOR_H
#define VECTOR_H

struct Vec4i
{
	Vec4i(int x, int y, int z, int w)
		: x(x)
		, y(y)
		, z(z)
		, w(w)
	{}

	int x, y, z, w;
};

#endif // VECTOR_H
