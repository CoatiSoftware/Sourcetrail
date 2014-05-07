#ifndef VECTOR_4_H
#define VECTOR_4_H

template<class T>
class Vector4
{
public:
	Vector4();
	Vector4(T x, T y, T z, T w);

	T x, y, z, w;
};

template<class T>
Vector4<T>::Vector4()
	: x(0)
	, y(0)
	, z(0)
	, w(0)
{}

template<class T>
Vector4<T>::Vector4(T x, T y, T z, T w)
	: x(x)
	, y(y)
	, z(z)
	, w(w)
{}

typedef Vector4<int> Vec4i;

#endif // VECTOR_4_H
