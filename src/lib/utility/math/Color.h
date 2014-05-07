#ifndef COLOR_H
#define COLOR_H

template<class T>
class Color
{
public:
	Color();
	Color(T r, T g, T b, T a);

	T r, g, b, a;
};

template<class T>
Color<T>::Color()
	: r(0)
	, g(0)
	, b(0)
	, a(0)
{}

template<class T>
Color<T>::Color(T r, T g, T b, T a)
	: r(r)
	, g(g)
	, b(b)
	, a(a)
{}

typedef Color<float> Colorf;
typedef Color<int> Colori;

#endif // COLOR_H
