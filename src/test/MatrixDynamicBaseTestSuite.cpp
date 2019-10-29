#include "catch.hpp"

#include "logging.h"
#include "MatrixDynamicBase.h"

namespace
{
	std::vector<std::vector<int>> getTestValues(const unsigned int numColumns, const unsigned int numRows)
	{
		std::vector<std::vector<int>> testValues;

		for (unsigned int x = 0; x < numColumns; x++)
		{
			std::vector<int> row;

			for (unsigned int y = 0; y < numRows; y++)
			{
				row.push_back(x + y);
			}

			testValues.push_back(row);
		}

		return testValues;
	}
}

TEST_CASE("matrixDynamicBase constructors")
{
	MatrixDynamicBase<int> matrix0;
	MatrixDynamicBase<int> matrix1(3, 5);

	std::vector<std::vector<int>> testValues = getTestValues(3, 5);
	MatrixDynamicBase<int> matrix2(testValues);

	REQUIRE(0 == matrix0.getColumnsCount());
	REQUIRE(0 == matrix0.getRowsCount());
	REQUIRE(3 == matrix1.getColumnsCount());
	REQUIRE(5 == matrix1.getRowsCount());
	REQUIRE(3 == matrix2.getColumnsCount());
	REQUIRE(5 == matrix2.getRowsCount());
}

TEST_CASE("matrixDynamicBase getValue setValue")
{
	std::vector<std::vector<int>> testValues = getTestValues(3, 5);
	MatrixDynamicBase<int> matrix0(testValues);

	REQUIRE(0 == matrix0.getValue(0, 0));
	REQUIRE(4 == matrix0.getValue(2, 2));
	REQUIRE(6 == matrix0.getValue(2, 4));

	matrix0.setValue(0, 0, 42);
	matrix0.setValue(2, 2, 84);
	matrix0.setValue(2, 4, 126);

	REQUIRE(42 == matrix0.getValue(0, 0));
	REQUIRE(84 == matrix0.getValue(2, 2));
	REQUIRE(126 == matrix0.getValue(2, 4));
}
