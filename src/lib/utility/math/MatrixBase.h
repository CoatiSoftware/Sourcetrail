#ifndef MATRIX_BASE_H
#define MATRIX_BASE_H

#include <cmath>
#include <sstream>
#include <stdexcept>
#include <string>

#include "VectorBase.h"

#define MATRIX_CHECK_INDEX(nIdx, mIdx) \
	do \
	{ \
		unsigned int n((nIdx)); \
		unsigned int m((mIdx)); \
		checkIndexInRange(n, m, __FUNCTION__); \
	} \
	while (0) \

template<class T, unsigned int N, unsigned int M>
class MatrixBase
{
public:
	MatrixBase();
	MatrixBase(const T values[N][M]);
	template<class U>
	MatrixBase(const MatrixBase<U, N, M>& matrix);
	~MatrixBase();

	T getValue(const unsigned int columnIndex, const unsigned int rowIndex) const;
	void setValue(const unsigned int columnIndex, const unsigned int rowIndex, const T& value);

	unsigned int getColumnsCount() const;
	unsigned int getRowsCount() const;

	MatrixBase<T, M, N> transposed() const;

	template<class U>
	void assign(const MatrixBase<U, N, M>& other);

	template<class U>
	MatrixBase<T, N, M> add(const MatrixBase<U, N, M>& other);
	template<class U>
	MatrixBase<T, N, M> subtract(const MatrixBase<U, N, M>& other);
	template<class U>
	MatrixBase<T, N, M> scalarMultiplication(const U& scalar);
	template<class U, unsigned int P>
	MatrixBase<T, P, M> matrixMultiplication(const MatrixBase<U, P, N>& other) const;

	// Checks whether all values are the same.
	template<class U>
	bool isEqual(const MatrixBase<U, N, M>& other) const;
	// Checks whether it really is the same object (at one and the same memory address).
	template<class U>
	bool isSame(const MatrixBase<U, N, M>& other) const;

	// jep, that's how you return an array...
	T (&operator[](const unsigned int index))[M];

	template<class U>
	void operator=(const MatrixBase<U, N, M>& other);

	template<class U>
	MatrixBase<U, N, M> operator+(const MatrixBase<U, N, M>& other) const;
	template<class U>
	MatrixBase<U, N, M> operator-(const MatrixBase<U, N, M>& other) const;
	template<class U>
	MatrixBase<U, N, M> operator*(const U& scalar) const;
	template<class U>
	MatrixBase<U, N, M> operator/(const U& scalar) const;

	template<class U>
	MatrixBase<U, N, M> operator+=(const MatrixBase<U, N, M>& other);
	template<class U>
	MatrixBase<U, N, M> operator-=(const MatrixBase<U, N, M>& other);
	template<class U>
	MatrixBase<U, N, M> operator*=(const U& scalar);
	template<class U>
	MatrixBase<U, N, M> operator/=(const U& scalar);

	// Checks whether all values are the same.
	template<class U>
	bool operator==(const MatrixBase<U, N, M>& other) const;
	// Checks whether at least one value is different.
	template<class U>
	bool operator!=(const MatrixBase<U, N, M>& other) const;

	std::string toString() const;

protected:
	T m_values[N][M];

private:
	inline void checkIndexInRange(unsigned int columnIndex, unsigned int rowIndex, const std::string& function) const
	{
		std::stringstream message;

		if (columnIndex >= N)
		{
			message << function << ": columnIndex " << columnIndex << " is out of range, maximum is " << N - 1;
		}

		if (rowIndex >= M)
		{
			if(message.str().length() > 0)
			{
				message << "\n";
			}

			message << function << ": rowIndex " << rowIndex << " is out of range, maximum is " << M - 1;
		}

		if(message.str().length() > 0)
		{
			throw std::range_error(message.str());
		}
	}

	template<class U>
	inline void setValues(const U values[N][M])
	{
		for (unsigned int i = 0; i < N; i++)
		{
			for(unsigned int j = 0; j < M; j++)
			{
				m_values[i][j] = (T)values[i][j];
			}
		}
	}

	template<class U>
	inline void setValues(const MatrixBase<U, N, M>& matrix)
	{
		for (unsigned int i = 0; i < N; i++)
		{
			for(unsigned int j = 0; j < M; j++)
			{
				m_values[i][j] = (T)matrix.getValue(i, j);
			}
		}
	}
};

template<class T, unsigned int N, unsigned int M>
MatrixBase<T, N, M>::MatrixBase()
{}

template<class T, unsigned int N, unsigned int M>
MatrixBase<T, N, M>::MatrixBase(const T values[N][M])
{
	setValues(values);
}

template<class T, unsigned int N, unsigned int M>
template<class U>
MatrixBase<T, N, M>::MatrixBase(const MatrixBase<U, N, M>& matrix)
{
	setValues(matrix);
}

template<class T, unsigned int N, unsigned int M>
MatrixBase<T, N, M>::~MatrixBase()
{
}

template<class T, unsigned int N, unsigned int M>
T MatrixBase<T, N, M>::getValue(const unsigned int columnIndex, const unsigned int rowIndex) const
{
	MATRIX_CHECK_INDEX(columnIndex, rowIndex);

	return m_values[columnIndex][rowIndex];
}

template<class T, unsigned int N, unsigned int M>
void MatrixBase<T, N, M>::setValue(const unsigned int columnIndex, const unsigned int rowIndex, const T& value)
{
	MATRIX_CHECK_INDEX(columnIndex, rowIndex);

	m_values[columnIndex][rowIndex] = value;
}

template<class T, unsigned int N, unsigned int M>
unsigned int MatrixBase<T, N, M>::getColumnsCount() const
{
	return N;
}

template<class T, unsigned int N, unsigned int M>
unsigned int MatrixBase<T, N, M>::getRowsCount() const
{
	return M;
}

template<class T, unsigned int N, unsigned int M>
MatrixBase<T, M, N> MatrixBase<T, N, M>::transposed() const
{
	T tmpValues[M][N];

	for(unsigned int i = 0; i < N; i++)
	{
		for(unsigned int j = 0; j < M; j++)
		{
			tmpValues[j][i] = m_values[i][j];
		}
	}

	return MatrixBase<T, M, N>(tmpValues);
}

template<class T, unsigned int N, unsigned int M>
template<class U>
void MatrixBase<T, N, M>::assign(const MatrixBase<U, N, M>& other)
{
	if(isSame(other))
	{
		return;
	}

	if(isEqual(other))
	{
		return;
	}

	setValues(other.m_values);
}

template<class T, unsigned int N, unsigned int M>
template<class U>
MatrixBase<T, N, M> MatrixBase<T, N, M>::add(const MatrixBase<U, N, M>& other)
{
	T tmpValues[N][M];
	for (unsigned int i = 0; i < N; i++)
	{
		for(unsigned int j = 0; j < M; j++)
		{
			tmpValues[i][j] = m_values[i][j] + other.m_values[i][j];
		}
	}

	// The values of *this won't be changed until they are all in a valid state.
	setValues(tmpValues);
	return *this;
}

template<class T, unsigned int N, unsigned int M>
template<class U>
MatrixBase<T, N, M> MatrixBase<T, N, M>::subtract(const MatrixBase<U, N, M>& other)
{
	T tmpValues[N][M];
	for (unsigned int i = 0; i < N; i++)
	{
		for(unsigned int j = 0; j < M; j++)
		{
			tmpValues[i][j] = m_values[i][j] - other.m_values[i][j];
		}
	}

	// The values of *this won't be changed until they are all in a valid state.
	setValues(tmpValues);
	return *this;
}

template<class T, unsigned int N, unsigned int M>
template<class U>
MatrixBase<T, N, M> MatrixBase<T, N, M>::scalarMultiplication(const U& scalar)
{
	T tmpValues[N][M];
	for (unsigned int i = 0; i < N; i++)
	{
		for(unsigned int j = 0; j < M; j++)
		{
			tmpValues[i][j] = m_values[i][j] * scalar;
		}
	}

	// The values of *this won't be changed until they are all in a valid state.
	setValues(tmpValues);
	return *this;
}

template<class T, unsigned int N, unsigned int M>
template<class U, unsigned int P>
MatrixBase<T, P, M> MatrixBase<T, N, M>::matrixMultiplication(const MatrixBase<U, P, N>& other) const
{
	MatrixBase<T, P, M> result;

	for(unsigned int m = 0; m < M; m++)
	{
		for(unsigned int p = 0; p < P; p++)
		{
			int val = 0;

			for(unsigned int n = 0; n < N; n++)
			{
				val += m_values[n][m] * other.getValue(p, n);
			}

			result.setValue(p, m, val);
		}
	}

	return result;
}

template<class T, unsigned int N, unsigned int M>
template<class U>
bool MatrixBase<T, N, M>::isEqual(const MatrixBase<U, N, M>& other) const
{
	for(unsigned int i = 0; i < N; i++)
	{
		for(unsigned int j = 0; j < M; j++)
		{
			if(m_values[i][j] != other.m_values[i][j])
			{
				return false;
			}
		}
	}

	return true;
}

template<class T, unsigned int N, unsigned int M>
template<class U>
bool MatrixBase<T, N, M>::isSame(const MatrixBase<U, N, M>& other) const
{
	return &other == this;
}

template<class T, unsigned int N, unsigned int M>
T (&MatrixBase<T, N, M>::operator[](const unsigned int index))[M]
{
	MATRIX_CHECK_INDEX(index, 0);

	return m_values[index];
}

template<class T, unsigned int N, unsigned int M>
template<class U>
void MatrixBase<T, N, M>::operator=(const MatrixBase<U, N, M>& other)
{
	assign(other);
}

template<class T, unsigned int N, unsigned int M>
template<class U>
MatrixBase<U, N, M> MatrixBase<T, N, M>::operator+(const MatrixBase<U, N, M>& other) const
{
	MatrixBase<T, N, M> result(*this);
	return result.add(other);
}

template<class T, unsigned int N, unsigned int M>
template<class U>
MatrixBase<U, N, M> MatrixBase<T, N, M>::operator-(const MatrixBase<U, N, M>& other) const
{
	MatrixBase<T, N, M> result(*this);
	return result.subtract(other);
}

template<class T, unsigned int N, unsigned int M>
template<class U>
MatrixBase<U, N, M> MatrixBase<T, N, M>::operator*(const U& scalar) const
{
	MatrixBase<T, N, M> result(*this);
	return result.scalarMultiplication(scalar);
}

template<class T, unsigned int N, unsigned int M>
template<class U>
MatrixBase<U, N, M> MatrixBase<T, N, M>::operator/(const U& scalar) const
{
	MatrixBase<T, N, M> result(*this);
	return result.scalarMultiplication(1.0f / scalar);
}

template<class T, unsigned int N, unsigned int M>
template<class U>
MatrixBase<U, N, M> MatrixBase<T, N, M>::operator+=(const MatrixBase<U, N, M>& other)
{
	return add(other);
}

template<class T, unsigned int N, unsigned int M>
template<class U>
MatrixBase<U, N, M> MatrixBase<T, N, M>::operator-=(const MatrixBase<U, N, M>& other)
{
	return subtract(other);
}

template<class T, unsigned int N, unsigned int M>
template<class U>
MatrixBase<U, N, M> MatrixBase<T, N, M>::operator*=(const U& scalar)
{
	return scalarMultiplication(scalar);
}

template<class T, unsigned int N, unsigned int M>
template<class U>
MatrixBase<U, N, M> MatrixBase<T, N, M>::operator/=(const U& scalar)
{
	return scalarMultiplication(1.0f / scalar);
}

template<class T, unsigned int N, unsigned int M>
template<class U>
bool MatrixBase<T, N, M>::operator==(const MatrixBase<U, N, M>& other) const
{
	return isEqual(other);
}

template<class T, unsigned int N, unsigned int M>
template<class U>
bool MatrixBase<T, N, M>::operator!=(const MatrixBase<U, N, M>& other) const
{
	return !isEqual(other);
}

template<class T, unsigned int N, unsigned int M>
std::string MatrixBase<T, N, M>::toString() const
{
	std::stringstream result;

	result << "\n";

	for(unsigned int j = 0; j < M; j++)
	{
		for(unsigned int i = 0; i < N; i++)
		{
			if(i > 0)
			{
				result << ", ";
			}

			result << m_values[i][j];
		}

		result << "\n";
	}

	return result.str();
}


template<class T, unsigned int N, unsigned int M>
std::ostream& operator<<(std::ostream& ostream, const MatrixBase<T, N, M>& matrix)
{
	ostream << matrix.toString();

	return ostream;
}

/**
 * @note Vector will be treated as column vector
 */
template<class T, class U, unsigned int N, unsigned int M>
VectorBase<U, M> multiply(MatrixBase<T, N, M>& matrix, const VectorBase<U, N>& vector)
{
	// vector will be stored in a matrix instance to make use of MatrixBase matrix multiplication
	MatrixBase<T, 1, N> vectorMatrix;
	for(unsigned int i = 0; i < vector.getDimensions(); i++)
	{
		vectorMatrix.setValue(0, i, vector.getValue(i));
	}

	MatrixBase<T, 1, M> resultMatrix = matrix.matrixMultiplication(vectorMatrix);

	VectorBase<U, M> result;
	for(unsigned int i = 0; i < result.getDimensions(); i++)
	{
		result.setValue(i, resultMatrix.getValue(0, i));
	}

	return result;
}

/**
 * @note Vector will be treated as row vector
 */
template<class T, class U, unsigned int N, unsigned int M>
VectorBase<U, N> multiply(const VectorBase<U, M>& vector, const MatrixBase<T, N, M>& matrix)
{
	// vector will be stored in a matrix instance to make use of MatrixBase matrix multiplication
	MatrixBase<T, M, 1> vectorMatrix;
	for(unsigned int i = 0; i < vector.getDimensions(); i++)
	{
		vectorMatrix.setValue(i, 0, vector.getValue(i));
	}

	MatrixBase<T, N, 1> resultMatrix = vectorMatrix.matrixMultiplication(matrix);

	VectorBase<U, N> result;
	for(unsigned int i = 0; i < result.getDimensions(); i++)
	{
		result.setValue(i, resultMatrix.getValue(i, 0));
	}

	return result;
}

/**
 * @note Vector will be treated as row vector
 */
template<class T, class U, unsigned int N, unsigned int M>
VectorBase<U, N> operator*(const VectorBase<U, M>& vector, const MatrixBase<T, N, M>& matrix)
{
	// vector will be stored in a matrix instance to make use of MatrixBase matrix multiplication
	MatrixBase<T, N, 1> vectorMatrix;
	for(unsigned int i = 0; i < vector.getDimensions(); i++)
	{
		vectorMatrix.setValue(i, 1, vector.getValue(i));
	}

	MatrixBase<T, N, 1> resultMatrix = vectorMatrix.matrixMultiplication(matrix);

	VectorBase<U, N> result;
	for(unsigned int i = 0; i < vector.getDimensions(); i++)
	{
		result.setValue(i, resultMatrix.getValue(i, 1));
	}

	return result;
}

#endif // MATRIX_BASE_H
