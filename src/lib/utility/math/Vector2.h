#ifndef VECTOR_2_H
#define VECTOR_2_H

#include <math.h>

template<class T>
class Vector2
{
public:
	Vector2();
	Vector2(const T x, const T y);
	Vector2(const Vector2<T>& vector);
	~Vector2();

	float getLengthSquared() const;
	float getLength() const;

	Vector2<T> normalize();
	Vector2<T> normalized() const;

	// checks whether all values are the same
	bool isEqual(const Vector2<T>& other) const;
	// checks if the memory address is the same
	bool isSame(const Vector2<T>& other) const;

	void operator=(Vector2<T>& other);

	Vector2<T> operator+(const Vector2<T>& other) const;
	Vector2<T> operator-(const Vector2<T>& other) const;
	Vector2<T> operator*(const T scalar) const;
	T operator*(const Vector2<T>& other) const;
	Vector2<T> operator/(const T scalar) const;

	Vector2<T> operator+=(const Vector2<T>& other);
	Vector2<T> operator-=(const Vector2<T>& other);
	Vector2<T> operator*=(const T scalar);
	Vector2<T> operator/=(const T scalar);

	// checks whether all values are the same
	bool operator==(const Vector2<T>& other) const;
	// checks whether at least one value is different
	bool operator!=(const Vector2<T>& other) const;

	T x;
	T y;
};

template<class T>
Vector2<T>::Vector2()
	: x(0)
	, y(0)
{
}

template<class T>
Vector2<T>::Vector2(const T x, const T y)
	: x(x)
	, y(y)
{
}

template<class T>
Vector2<T>::Vector2(const Vector2<T> &vector)
	: x(vector.x)
	, y(vector.y)
{
}

template<class T>
Vector2<T>::~Vector2()
{
}

template<class T>
float Vector2<T>::getLengthSquared() const
{
	return (x * x) + (y * y);
}

template<class T>
float Vector2<T>::getLength() const
{
	return std::sqrt(getLengthSquared());
}

template<class T>
Vector2<T> Vector2<T>::normalize()
{
	float length = getLength();

	x /= length;
	y /= length;

	return *this;
}

template<class T>
Vector2<T> Vector2<T>::normalized() const
{
	float length = getLength();

	return Vector2<T>(static_cast<float>(x) / length, static_cast<float>(y) / length);
}

template<class T>
bool Vector2<T>::isEqual(const Vector2<T>& other) const
{
	return ((x == other.x) && (y == other.y));
}

template<class T>
bool Vector2<T>::isSame(const Vector2<T>& other) const
{
	return &other == this;
}

template<class T>
void Vector2<T>::operator=(Vector2<T>& other)
{
	if (isSame(other) || isEqual(other))
	{
		return;
	}

	x = other.x;
	y = other.y;
}

template<class T>
Vector2<T> Vector2<T>::operator+(const Vector2<T>& other) const
{
	Vector2<T> result(*this);
	return (result += other);
}

template<class T>
Vector2<T> Vector2<T>::operator-(const Vector2<T>& other) const
{
	Vector2<T> result(*this);
	return (result -= other);
}

template<class T>
Vector2<T> Vector2<T>::operator*(const T scalar) const
{
	Vector2<T> result(*this);
	return (result *= scalar);
}

template<class T>
T Vector2<T>::operator*(const Vector2<T>& other) const
{
	return x * other.x + y * other.y;
}

template<class T>
Vector2<T> Vector2<T>::operator/(const T scalar) const
{
	Vector2<T> result(*this);
	return (result /= scalar);
}

template<class T>
Vector2<T> Vector2<T>::operator+=(const Vector2<T>& other)
{
	x += other.x;
	y += other.y;

	return *this;
}

template<class T>
Vector2<T> Vector2<T>::operator-=(const Vector2<T>& other)
{
	x -= other.x;
	y -= other.y;

	return *this;
}

template<class T>
Vector2<T> Vector2<T>::operator*=(const T scalar)
{
	x *= scalar;
	y *= scalar;

	return *this;
}

template<class T>
Vector2<T> Vector2<T>::operator/=(const T scalar)
{
	x /= scalar;
	y /= scalar;

	return *this;
}

template<class T>
bool Vector2<T>::operator==(const Vector2<T>& other) const
{
	return isEqual(other);
}

template<class T>
bool Vector2<T>::operator!=(const Vector2<T>& other) const
{
	return !isEqual(other);
}


template<class T>
std::ostream& operator<<(std::ostream& ostream, const Vector2<T>& vector)
{
	ostream << "[" << vector.x << ", " << vector.y << "]";
	return ostream;
}

typedef Vector2<float> Vec2f;
typedef Vector2<int> Vec2i;

#endif // VECTOR_2_H
