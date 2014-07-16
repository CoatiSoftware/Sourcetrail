#ifndef VECTOR_BASE_H
#define VECTOR_BASE_H

#include <cmath>
#include <sstream>
#include <stdexcept>
#include <string>

#define VECTOR_CHECK_INDEX(idx) \
	do \
	{ \
		unsigned int i((idx)); \
		checkIndexInRange(i, __FUNCTION__); \
	} \
	while (0) \


template<class T, unsigned int N>
class VectorBase
{
public:
	VectorBase();
	VectorBase(const T values[N]);
	VectorBase(const VectorBase<T, N>& vector);
	~VectorBase();

	T getValue(const unsigned int index) const;
	void setValue(const unsigned int index, const T& value);

	unsigned int getDimensions() const;

	float getLengthSquared() const;
	float getLength() const;

	VectorBase<T, N> normalize();
	VectorBase<T, N> normalized() const;

	template<class U>
	void assign(const VectorBase<U, N>& other);

	template<class U>
	VectorBase<T, N> add(const VectorBase<U, N>& other);
	template<class U>
	VectorBase<T, N> subtract(const VectorBase<U, N>& other);
	template<class U>
	VectorBase<T, N> scalarMultiplication(const U& scalar);
	template<class U>
	T dotProduct(const VectorBase<U, N>& other);

	// Checks whether all values are the same.
	template<class U>
	bool isEqual(const VectorBase<U, N>& other) const;
	// Checks whether it really is the same object (at one and the same memory address).
	template<class U>
	bool isSame(const VectorBase<U, N>& other) const;

	T operator[](const unsigned int index);

	template<class U>
	void operator=(const VectorBase<U, N>& other);

	template<class U>
	VectorBase<T, N> operator+(const VectorBase<U, N>& other) const;
	template<class U>
	VectorBase<T, N> operator-(const VectorBase<U, N>& other) const;
	template<class U>
	VectorBase<T, N> operator*(const U& scalar) const;
	/*template<class U>
	T operator*(const VectorBase<U, N>& other) const;*/
	template<class U>
	VectorBase<T, N> operator/(const U& scalar) const;

	template<class U>
	VectorBase<T, N> operator+=(const VectorBase<U, N>& other);
	template<class U>
	VectorBase<T, N> operator-=(const VectorBase<U, N>& other);
	template<class U>
	VectorBase<T, N> operator*=(const U& scalar);
	template<class U>
	VectorBase<T, N> operator/=(const U& scalar);

	// Checks whether all values are the same.
	template<class U>
	bool operator==(const VectorBase<U, N>& other) const;
	// Checks whether at least one value is different.
	template<class U>
	bool operator!=(const VectorBase<U, N>& other) const;

	std::string toString() const;

protected:
	T m_values[N];

private:
	inline void checkIndexInRange(unsigned int index, const std::string& function) const
	{
		if (index >= N)
		{
			std::stringstream message;
			message << function << ": index " << index << " is out of range, maximum is " << N - 1;
			throw std::range_error(message.str());
		}
	}

	inline void setValues(const T values[N])
	{
		for (unsigned int i = 0; i < N; i++)
		{
			m_values[i] = values[i];
		}
	}
};


template<class T, unsigned int N>
VectorBase<T, N>::VectorBase()
{}

template<class T, unsigned int N>
VectorBase<T, N>::VectorBase(const T values[N])
{
	setValues(values);
}

template<class T, unsigned int N>
VectorBase<T, N>::VectorBase(const VectorBase<T, N>& vector)
{
	setValues(vector.m_values);
}

template<class T, unsigned int N>
VectorBase<T, N>::~VectorBase()
{
}

template<class T, unsigned int N>
T VectorBase<T, N>::getValue(const unsigned int index) const
{
	VECTOR_CHECK_INDEX(index);

	return m_values[index];
}

template<class T, unsigned int N>
void VectorBase<T, N>::setValue(const unsigned int index, const T& value)
{
	VECTOR_CHECK_INDEX(index);

	m_values[index] = value;
}

template<class T, unsigned int N>
unsigned int VectorBase<T, N>::getDimensions() const
{
	return N;
}

template<class T, unsigned int N>
float VectorBase<T, N>::getLengthSquared() const
{
	float result = 0.0f;

	for (unsigned int i = 0; i < N; i++)
	{
		result += float(m_values[i] * m_values[i]);
	}

	return result;
}

template<class T, unsigned int N>
float VectorBase<T, N>::getLength() const
{
	return std::sqrt(getLengthSquared());
}

template<class T, unsigned int N>
VectorBase<T, N> VectorBase<T, N>::normalize()
{
	float length = getLength();

	if (length > 0.0f)
	{
		T tmpValues[N];
		for (unsigned int i = 0; i < N; i++)
		{
			tmpValues[i] = m_values[i] / length;
		}

		// The values of *this won't be changed until all are in a valid state.
		setValues(tmpValues);
	}

	return *this;
}

template<class T, unsigned int N>
VectorBase<T, N> VectorBase<T, N>::normalized() const
{
	float length = getLength();

	T tmpValues[N];

	if (length > 0.0f)
	{
		for (unsigned int i = 0; i < N; i++)
		{
			tmpValues[i] = m_values[i] / length;
		}
	}
	else
	{
		for (unsigned int i = 0; i < N; i++)
		{
			tmpValues[i] = 0;
		}
	}

	return VectorBase<T, N>(tmpValues);
}

template<class T, unsigned int N>
template<class U>
void VectorBase<T, N>::assign(const VectorBase<U, N>& other)
{
	if (isSame(other))
	{
		return;
	}

	if (isEqual(other))
	{
		return;
	}

	setValues(other.m_values);
}

template<class T, unsigned int N>
template<class U>
VectorBase<T, N> VectorBase<T, N>::add(const VectorBase<U, N>& other)
{
	T tmpValues[N];
	for (unsigned int i = 0; i < N; i++)
	{
		tmpValues[i] = m_values[i] + other.m_values[i];
	}

	// The values of *this won't be changed until all are in a valid state.
	setValues(tmpValues);
	return *this;
}

template<class T, unsigned int N>
template<class U>
VectorBase<T, N> VectorBase<T, N>::subtract(const VectorBase<U, N>& other)
{
	T tmpValues[N];
	for (unsigned int i = 0; i < N; i++)
	{
		tmpValues[i] = m_values[i] - other.m_values[i];
	}

	// The values of *this won't be changed until all are in a valid state.
	setValues(tmpValues);
	return *this;
}

template<class T, unsigned int N>
template<class U>
VectorBase<T, N> VectorBase<T, N>::scalarMultiplication(const U& scalar)
{
	T tmpValues[N];
	for (unsigned int i = 0; i < N; i++)
	{
		tmpValues[i] = m_values[i] * scalar;
	}

	// The values of *this won't be changed until all are in a valid state.
	setValues(tmpValues);
	return *this;
}

template<class T, unsigned int N>
template<class U>
T VectorBase<T, N>::dotProduct(const VectorBase<U, N>& other)
{
	T result = 0.0f;
	for (unsigned int i = 0; i < N; i++)
	{
		result += (m_values[i] * other.m_values[i]);
	}
	return result;
}

template<class T, unsigned int N>
template<class U>
bool VectorBase<T, N>::isEqual(const VectorBase<U, N>& other) const
{
	for (unsigned int i = 0; i < N; i++)
	{
		if (m_values[i] != other.m_values[i])
		{
			return false;
		}
	}

	return true;
}

template<class T, unsigned int N>
template<class U>
bool VectorBase<T, N>::isSame(const VectorBase<U, N>& other) const
{
	return &other == this;
}

template<class T, unsigned int N>
T VectorBase<T, N>::operator[](const unsigned int index)
{
	VECTOR_CHECK_INDEX(index);

	return m_values[index];
}

template<class T, unsigned int N>
template<class U>
void VectorBase<T, N>::operator=(const VectorBase<U, N>& other)
{
	assign(other);
}

template<class T, unsigned int N>
template<class U>
VectorBase<T, N> VectorBase<T, N>::operator+(const VectorBase<U, N>& other) const
{
	VectorBase<T, N> result(*this);
	return result.add(other);
}

template<class T, unsigned int N>
template<class U>
VectorBase<T, N> VectorBase<T, N>::operator-(const VectorBase<U, N>& other) const
{
	VectorBase<T, N> result(*this);
	return result.subtract(other);
}

template<class T, unsigned int N>
template<class U>
VectorBase<T, N> VectorBase<T, N>::operator*(const U& scalar) const
{
	VectorBase<T, N> result(*this);
	return result.scalarMultiplication(scalar);
}

//template<class T, unsigned int N>
//template<class U>
//T VectorBase<T, N>::operator*(const VectorBase<U, N>& other) const
//{
//	return dotProduct(other);
//}

template<class T, unsigned int N>
template<class U>
VectorBase<T, N> VectorBase<T, N>::operator/(const U& scalar) const
{
	VectorBase<T, N> result(*this);
	return result.scalarMultiplication(1.0f / scalar);
}

template<class T, unsigned int N>
template<class U>
VectorBase<T, N> VectorBase<T, N>::operator+=(const VectorBase<U, N>& other)
{
	return add(other);
}

template<class T, unsigned int N>
template<class U>
VectorBase<T, N> VectorBase<T, N>::operator-=(const VectorBase<U, N>& other)
{
	return subtract(other);
}

template<class T, unsigned int N>
template<class U>
VectorBase<T, N> VectorBase<T, N>::operator*=(const U& scalar)
{
	return scalarMultiplication(scalar);
}

template<class T, unsigned int N>
template<class U>
VectorBase<T, N> VectorBase<T, N>::operator/=(const U& scalar)
{
	return scalarMultiplication(1.0f / scalar);
}

template<class T, unsigned int N>
template<class U>
bool VectorBase<T, N>::operator==(const VectorBase<U, N>& other) const
{
	return isEqual(other);
}

template<class T, unsigned int N>
template<class U>
bool VectorBase<T, N>::operator!=(const VectorBase<U, N>& other) const
{
	return !isEqual(other);
}

template<class T, unsigned int N>
std::string VectorBase<T, N>::toString() const
{
	std::stringstream result;
	result << "[";

	for (unsigned int i = 0; i < N - 1; i++)
	{
		result << m_values[i] << ", ";
	}
	result << m_values[N - 1] << "]";

	return result.str();
}

template<class T, unsigned int N>
std::ostream& operator<<(std::ostream& ostream, const VectorBase<T, N>& vector)
{
	ostream << vector.toString();

	return ostream;
}

#endif // VECTOR_BASE_H
