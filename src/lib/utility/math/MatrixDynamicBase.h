#ifndef MATRIX_DYNAMIC_BASE_H
#define MATRIX_DYNAMIC_BASE_H

#include <sstream>
#include <string>
#include <vector>

/**
 * @brief Matrix of variable size, needed for spectral graph layouting
 * @note Use MatrixBase whenever possible because it's more efficient (e.g. it doesn't use stl
 * containers)
 */
template <class T>
class MatrixDynamicBase
{
public:
	MatrixDynamicBase();
	MatrixDynamicBase(const unsigned int numColumns, const unsigned int numRows);
	MatrixDynamicBase(const std::vector<std::vector<T>>& values);
	~MatrixDynamicBase();

	T getValue(const unsigned int columnIndex, const unsigned int rowIndex) const;
	void setValue(const unsigned int columnIndex, const unsigned int rowIndex, const T& value);

	unsigned int getColumnsCount() const;
	unsigned int getRowsCount() const;

	std::string toString() const;

private:
	void initializeValues(const unsigned int numColumns, const unsigned int numRows);

	std::vector<std::vector<T>> m_values;
};

template <class T>
MatrixDynamicBase<T>::MatrixDynamicBase()
{
}

template <class T>
MatrixDynamicBase<T>::MatrixDynamicBase(const unsigned int numColumns, const unsigned int numRows)
{
	initializeValues(numColumns, numRows);
}

template <class T>
MatrixDynamicBase<T>::MatrixDynamicBase(const std::vector<std::vector<T>>& values): m_values(values)
{
}

template <class T>
MatrixDynamicBase<T>::~MatrixDynamicBase()
{
}

template <class T>
T MatrixDynamicBase<T>::getValue(const unsigned int columnIndex, const unsigned int rowIndex) const
{
	return m_values[columnIndex][rowIndex];
}

template <class T>
void MatrixDynamicBase<T>::setValue(
	const unsigned int columnIndex, const unsigned int rowIndex, const T& value)
{
	m_values[columnIndex][rowIndex] = value;
}

template <class T>
unsigned int MatrixDynamicBase<T>::getColumnsCount() const
{
	return static_cast<unsigned int>(m_values.size());
}

template <class T>
unsigned int MatrixDynamicBase<T>::getRowsCount() const
{
	if (m_values.size() > 0)
	{
		return static_cast<unsigned int>(m_values[0].size());
	}

	return 0;
}

template <class T>
std::string MatrixDynamicBase<T>::toString() const
{
	std::stringstream result;

	result << "\n";

	unsigned int rowCount = getRowsCount();
	unsigned int columnCount = getColumnsCount();

	for (unsigned int j = 0; j < rowCount; j++)
	{
		for (unsigned int i = 0; i < columnCount; i++)
		{
			if (i > 0)
			{
				result << ", ";
			}

			result << m_values[i][j];
		}

		result << "\n";
	}

	return result.str();
}

template <class T>
void MatrixDynamicBase<T>::initializeValues(const unsigned int numColumns, const unsigned int numRows)
{
	for (unsigned int x = 0; x < numColumns; x++)
	{
		std::vector<T> row;
		for (unsigned int y = 0; y < numRows; y++)
		{
			row.push_back(0);
		}
		m_values.push_back(row);
	}
}

template <class T>
std::ostream& operator<<(std::ostream& ostream, const MatrixDynamicBase<T>& matrix)
{
	ostream << matrix.toString();

	return ostream;
}

#endif	  // MATRIX_DYNAMIC_BASE_H
