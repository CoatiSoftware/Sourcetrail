#include "TestSuite.h"

#include "logging.h"
#include "MatrixDynamicBase.h"

class MatrixDynamicBaseTestSuite : public CxxTest::TestSuite
{
public:
	void test_matrixDynamicBase_constructors()
	{
		MatrixDynamicBase<int> matrix0;
		MatrixDynamicBase<int> matrix1(3, 5);

		std::vector<std::vector<int>> testValues = getTestValues(3, 5);
		MatrixDynamicBase<int> matrix2(testValues);

		TS_ASSERT_EQUALS(0, matrix0.getColumnsCount());
		TS_ASSERT_EQUALS(0, matrix0.getRowsCount());
		TS_ASSERT_EQUALS(3, matrix1.getColumnsCount());
		TS_ASSERT_EQUALS(5, matrix1.getRowsCount());
		TS_ASSERT_EQUALS(3, matrix2.getColumnsCount());
		TS_ASSERT_EQUALS(5, matrix2.getRowsCount());
	}

	void test_matrixDynamicBase_getValue_setValue()
	{
		std::vector<std::vector<int>> testValues = getTestValues(3, 5);
		MatrixDynamicBase<int> matrix0(testValues);

		TS_ASSERT_EQUALS(0, matrix0.getValue(0, 0));
		TS_ASSERT_EQUALS(4, matrix0.getValue(2, 2));
		TS_ASSERT_EQUALS(6, matrix0.getValue(2, 4));

		matrix0.setValue(0, 0, 42);
		matrix0.setValue(2, 2, 84);
		matrix0.setValue(2, 4, 126);

		TS_ASSERT_EQUALS(42, matrix0.getValue(0, 0));
		TS_ASSERT_EQUALS(84, matrix0.getValue(2, 2));
		TS_ASSERT_EQUALS(126, matrix0.getValue(2, 4));
	}

private:
	std::vector<std::vector<int>> getTestValues(const unsigned int numColumns, const unsigned int numRows)
	{
		std::vector<std::vector<int>> testValues;

		for(unsigned int x = 0; x < numColumns; x++)
		{
			std::vector<int> row;

			for(unsigned int y = 0; y < numRows; y++)
			{
				row.push_back(x + y);
			}

			testValues.push_back(row);
		}

		return testValues;
	}
};