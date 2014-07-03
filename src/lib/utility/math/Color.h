#ifndef COLOR_H
#define COLOR_H

#include <string>
#include <sstream>

template<class T>
class Color
{
public:
	static Color<T> fromString(std::string str);

	Color();
	Color(T r, T g, T b, T a);

	std::string toString() const;

	T r, g, b, a;
};

template<class T>
Color<T> Color<T>::fromString(std::string str)
{
	Color<T> color;
	std::stringstream ss;
	ss << str;
	ss >> color.r >> color.g >> color.b >> color.a;
	return color;
}

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

template<class T>
std::string Color<T>::toString() const
{
	std::stringstream ss;
	ss << r << ' ' << g << ' ' << b << ' ' << a;
	return ss.str();
}

typedef Color<float> Colorf;
typedef Color<int> Colori;

#endif // COLOR_H
