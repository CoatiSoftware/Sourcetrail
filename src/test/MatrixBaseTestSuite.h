#include "cxxtest/TestSuite.h"

#include "utility/logging/logging.h"
#include "utility/math/MatrixBase.h"
#include "utility/math/VectorBase.h"

class MatrixBaseTestSuite : public CxxTest::TestSuite
{
public:
	void test_matrixBase_constructors()
	{
		MatrixBase<int, 4, 5> matrix0;

		TS_ASSERT_EQUALS(4, matrix0.getColumnsCount());
		TS_ASSERT_EQUALS(5, matrix0.getRowsCount());

		Array3x5<int> testValues = getTestValues3x5();

		MatrixBase<int, 3, 5> matrix1(testValues.array);

		TS_ASSERT_EQUALS(3, matrix1.getColumnsCount());
		TS_ASSERT_EQUALS(5, matrix1.getRowsCount());

		TS_ASSERT_EQUALS(0, matrix1.getValue(0, 0));
		TS_ASSERT_EQUALS(6, matrix1.getValue(2, 4));

		MatrixBase<int, 3, 5> matrix2(matrix1);

		TS_ASSERT_EQUALS(0, matrix2.getValue(0, 0));
		TS_ASSERT_EQUALS(6, matrix2.getValue(2, 4));
	}

	void test_matrixBase_getSetValue()
	{
		MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();

		int value2_2 = matrix0.getValue(2, 2);
		matrix0.setValue(2, 2, value2_2*2);
		TS_ASSERT_EQUALS(value2_2*2, matrix0.getValue(2, 2));
		TS_ASSERT_EQUALS(3, matrix0.getValue(1, 2));
		TS_ASSERT_EQUALS(0, matrix0.getValue(0, 0));
	}

	void test_matrixBase_getRowsColumnsCount()
	{
		MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();
		MatrixBase<int, 5, 3> matrix1 = getTestMatrix5x3();

		TS_ASSERT_EQUALS(3, matrix0.getColumnsCount());
		TS_ASSERT_EQUALS(5, matrix0.getRowsCount());

		TS_ASSERT_EQUALS(5, matrix1.getColumnsCount());
		TS_ASSERT_EQUALS(3, matrix1.getRowsCount());
	}

	void test_matrixBase_transposed()
	{
		MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();
		TS_ASSERT_EQUALS(3, matrix0.getColumnsCount());
		TS_ASSERT_EQUALS(5, matrix0.getRowsCount());

		MatrixBase<int, 5, 3> matrix1 = matrix0.transposed();
		TS_ASSERT_EQUALS(5, matrix1.getColumnsCount());
		TS_ASSERT_EQUALS(3, matrix1.getRowsCount());

		TS_ASSERT_EQUALS(matrix0.getValue(0, 0), matrix1.getValue(0, 0));
		TS_ASSERT_EQUALS(matrix0.getValue(0, 1), matrix1.getValue(1, 0));
		TS_ASSERT_EQUALS(matrix0.getValue(0, 4), matrix1.getValue(4, 0));
		TS_ASSERT_EQUALS(matrix0.getValue(1, 4), matrix1.getValue(4, 1));
		TS_ASSERT_EQUALS(matrix0.getValue(2, 4), matrix1.getValue(4, 2));
		TS_ASSERT_EQUALS(matrix0.getValue(0, 3), matrix1.getValue(3, 0));
		TS_ASSERT_EQUALS(matrix0.getValue(1, 3), matrix1.getValue(3, 1));
		TS_ASSERT_EQUALS(matrix0.getValue(2, 3), matrix1.getValue(3, 2));
	}

	void test_matrixBase_assign()
	{
		MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();
		MatrixBase<int, 3, 5> matrix1 = getTestMatrix3x5_b();

		TS_ASSERT_EQUALS(0, matrix0.getValue(0, 0));
		TS_ASSERT_EQUALS(2, matrix0.getValue(1, 1));
		TS_ASSERT_EQUALS(4, matrix0.getValue(2, 2));

		TS_ASSERT_EQUALS(0, matrix1.getValue(0, 0));
		TS_ASSERT_EQUALS(-2, matrix1.getValue(1, 1));
		TS_ASSERT_EQUALS(-4, matrix1.getValue(2, 2));

		matrix0.assign(matrix1);

		TS_ASSERT_EQUALS(0, matrix0.getValue(0, 0));
		TS_ASSERT_EQUALS(-2, matrix0.getValue(1, 1));
		TS_ASSERT_EQUALS(-4, matrix0.getValue(2, 2));
	}

	void test_matrixBase_add_subtract()
	{
		MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();
		MatrixBase<int, 3, 5> matrix1 = getTestMatrix3x5_b();

		matrix0.add(matrix1);

		TS_ASSERT_EQUALS(0, matrix0.getValue(0, 0));
		TS_ASSERT_EQUALS(0, matrix0.getValue(1, 1));
		TS_ASSERT_EQUALS(0, matrix0.getValue(2, 2));

		matrix0.subtract(matrix1);

		TS_ASSERT_EQUALS(0, matrix0.getValue(0, 0));
		TS_ASSERT_EQUALS(2, matrix0.getValue(1, 1));
		TS_ASSERT_EQUALS(4, matrix0.getValue(2, 2));
	}

	void test_matrixBase_multiplyDivideScalar()
	{
		MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();

		matrix0.scalarMultiplication(2.0f); // float is on porpoise (so is porpoise, womp womp)

		TS_ASSERT_EQUALS(0, matrix0.getValue(0, 0));
		TS_ASSERT_EQUALS(4, matrix0.getValue(1, 1));
		TS_ASSERT_EQUALS(8, matrix0.getValue(2, 2));

		matrix0.scalarMultiplication(0.5f);

		TS_ASSERT_EQUALS(0, matrix0.getValue(0, 0));
		TS_ASSERT_EQUALS(2, matrix0.getValue(1, 1));
		TS_ASSERT_EQUALS(4, matrix0.getValue(2, 2));

		matrix0.scalarMultiplication(0.5f);

		TS_ASSERT_EQUALS(0, matrix0.getValue(0, 0));
		TS_ASSERT_EQUALS(1, matrix0.getValue(1, 1));
		TS_ASSERT_EQUALS(2, matrix0.getValue(2, 2));

		matrix0.scalarMultiplication(0.5f);

		TS_ASSERT_EQUALS(0, matrix0.getValue(0, 0));
		TS_ASSERT_EQUALS(0, matrix0.getValue(1, 1));
		TS_ASSERT_EQUALS(1, matrix0.getValue(2, 2));
	}

	void test_matrixBase_multiplyMatrix()
	{
		MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();
		MatrixBase<int, 3, 5> matrix1 = getTestMatrix3x5_b();
		MatrixBase<int, 5, 3> matrix1t = matrix1.transposed();

		MatrixBase<int, 5, 5> matrix2 = matrix0.matrixMultiplication(matrix1t);
		MatrixBase<int, 3, 3> matrix3 = matrix1t.matrixMultiplication(matrix0);

		// expected results
		// matrix0 * matrix1t
		/**
		 * -5, -8, -11, -14, -17
		 * -8, -14, -20, -26, -32
		 * -11, -20, -29, -38, -47
		 * -14, -26, -38, -50, -62
		 * -17, -32, -47, -62, -77
		 */

		// matrix1t * matrix0
		/**
		 * -30, -40, -50
		 * -40, -55, -70
		 * -50, -70, -90
		 */


		TS_ASSERT_EQUALS(5, matrix2.getColumnsCount());
		TS_ASSERT_EQUALS(5, matrix2.getRowsCount());

		TS_ASSERT_EQUALS(3, matrix3.getColumnsCount());
		TS_ASSERT_EQUALS(3, matrix3.getRowsCount());

		TS_ASSERT_EQUALS(-5, matrix2.getValue(0, 0));
		TS_ASSERT_EQUALS(-77, matrix2.getValue(4, 4));
		TS_ASSERT_EQUALS(-29, matrix2.getValue(2, 2));
		TS_ASSERT_EQUALS(-11, matrix2.getValue(2, 0));
		TS_ASSERT_EQUALS(-11, matrix2.getValue(0, 2));
		TS_ASSERT_EQUALS(-38, matrix2.getValue(3, 2));


		TS_ASSERT_EQUALS(-30, matrix3.getValue(0, 0));
		TS_ASSERT_EQUALS(-90, matrix3.getValue(2, 2));
		TS_ASSERT_EQUALS(-50, matrix3.getValue(2, 0));
		TS_ASSERT_EQUALS(-50, matrix3.getValue(0, 2));
		TS_ASSERT_EQUALS(-55, matrix3.getValue(1, 1));
	}

	void test_matrixBase_isEqual()
	{
		MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();
		MatrixBase<int, 3, 5> matrix0_b = getTestMatrix3x5();
		MatrixBase<int, 3, 5> matrix1 = getTestMatrix3x5_b();

		TS_ASSERT_EQUALS(true, matrix0.isEqual(matrix0_b));
		TS_ASSERT_EQUALS(false, matrix0.isEqual(matrix1));
		TS_ASSERT_EQUALS(true, matrix0.isEqual(matrix0));
	}

	void test_matrixBase_isSame()
	{
		MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();
		MatrixBase<int, 3, 5> matrix0_b = getTestMatrix3x5();
		MatrixBase<int, 3, 5> matrix1 = getTestMatrix3x5_b();

		TS_ASSERT_EQUALS(false, matrix0.isSame(matrix0_b));
		TS_ASSERT_EQUALS(false, matrix0.isSame(matrix1));
		TS_ASSERT_EQUALS(true, matrix0.isSame(matrix0));
	}

	void test_matrixBase_accessOperator()
	{
		MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();

		TS_ASSERT_EQUALS(0, matrix0[0][0]);
		TS_ASSERT_EQUALS(4, matrix0[2][2]);
		TS_ASSERT_EQUALS(6, matrix0[2][4]);

		matrix0[0][0] = 42;

		TS_ASSERT_EQUALS(42, matrix0[0][0]);
		TS_ASSERT_EQUALS(4, matrix0[2][2]);
		TS_ASSERT_EQUALS(6, matrix0[2][4]);
	}

	void test_matrixBase_operators()
	{
		MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();
		MatrixBase<int, 3, 5> matrix0_b = getTestMatrix3x5_b();

		TS_ASSERT_EQUALS(0, matrix0.getValue(0, 0));
		TS_ASSERT_EQUALS(4, matrix0.getValue(2, 2));
		TS_ASSERT_EQUALS(6, matrix0.getValue(2, 4));

		TS_ASSERT_EQUALS(0, matrix0_b.getValue(0, 0));
		TS_ASSERT_EQUALS(-4, matrix0_b.getValue(2, 2));
		TS_ASSERT_EQUALS(-6, matrix0_b.getValue(2, 4));

		MatrixBase<int, 3, 5> matrix1 = matrix0 + matrix0_b;
		MatrixBase<int, 3, 5> matrix2 = matrix0 - matrix0_b;

		TS_ASSERT_EQUALS(0, matrix1.getValue(0, 0));
		TS_ASSERT_EQUALS(0, matrix1.getValue(2, 2));
		TS_ASSERT_EQUALS(0, matrix1.getValue(2, 4));

		TS_ASSERT_EQUALS(0, matrix2.getValue(0, 0));
		TS_ASSERT_EQUALS(8, matrix2.getValue(2, 2));
		TS_ASSERT_EQUALS(12, matrix2.getValue(2, 4));

		MatrixBase<int, 3, 5> matrix3 = matrix0 * 3;
		MatrixBase<int, 3, 5> matrix4 = matrix0 / 2;
		MatrixBase<int, 3, 5> matrix5 = matrix0 * 3.3f; // float is on purpose

		TS_ASSERT_EQUALS(0, matrix3.getValue(0, 0));
		TS_ASSERT_EQUALS(12, matrix3.getValue(2, 2));
		TS_ASSERT_EQUALS(18, matrix3.getValue(2, 4));

		TS_ASSERT_EQUALS(0, matrix4.getValue(0, 0));
		TS_ASSERT_EQUALS(2, matrix4.getValue(2, 2));
		TS_ASSERT_EQUALS(3, matrix4.getValue(2, 4));

		TS_ASSERT_EQUALS(0, matrix5.getValue(0, 0));
		TS_ASSERT_EQUALS(13, matrix5.getValue(2, 2));
		TS_ASSERT_EQUALS(19, matrix5.getValue(2, 4));
	}

	void test_matrixBase_assignOperators()
	{
		MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();
		MatrixBase<int, 3, 5> matrix0_b = getTestMatrix3x5_b();

		TS_ASSERT_EQUALS(0, matrix0.getValue(0, 0));
		TS_ASSERT_EQUALS(4, matrix0.getValue(2, 2));
		TS_ASSERT_EQUALS(6, matrix0.getValue(2, 4));

		TS_ASSERT_EQUALS(0, matrix0_b.getValue(0, 0));
		TS_ASSERT_EQUALS(-4, matrix0_b.getValue(2, 2));
		TS_ASSERT_EQUALS(-6, matrix0_b.getValue(2, 4));

		MatrixBase<int, 3, 5> matrix1 = getTestMatrix3x5();
		matrix1 += matrix0;

		TS_ASSERT_EQUALS(0, matrix1.getValue(0, 0));
		TS_ASSERT_EQUALS(8, matrix1.getValue(2, 2));
		TS_ASSERT_EQUALS(12, matrix1.getValue(2, 4));

		matrix1 += matrix0_b;

		TS_ASSERT_EQUALS(0, matrix1.getValue(0, 0));
		TS_ASSERT_EQUALS(4, matrix1.getValue(2, 2));
		TS_ASSERT_EQUALS(6, matrix1.getValue(2, 4));

		matrix1 -= matrix0_b;

		TS_ASSERT_EQUALS(0, matrix1.getValue(0, 0));
		TS_ASSERT_EQUALS(8, matrix1.getValue(2, 2));
		TS_ASSERT_EQUALS(12, matrix1.getValue(2, 4));

		matrix1 *= 3.3f;

		TS_ASSERT_EQUALS(0, matrix1.getValue(0, 0));
		TS_ASSERT_EQUALS(26, matrix1.getValue(2, 2));
		TS_ASSERT_EQUALS(39, matrix1.getValue(2, 4));

		matrix1 /= 3;

		TS_ASSERT_EQUALS(0, matrix1.getValue(0, 0));
		TS_ASSERT_EQUALS(8, matrix1.getValue(2, 2));
		TS_ASSERT_EQUALS(13, matrix1.getValue(2, 4));
	}

	void test_matrixBase_comparisonOperators()
	{
		MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();
		MatrixBase<int, 3, 5> matrix0_b = getTestMatrix3x5_b();
		MatrixBase<int, 3, 5> matrix1 = getTestMatrix3x5();

		TS_ASSERT_EQUALS(0, matrix0.getValue(0, 0));
		TS_ASSERT_EQUALS(4, matrix0.getValue(2, 2));
		TS_ASSERT_EQUALS(6, matrix0.getValue(2, 4));

		TS_ASSERT_EQUALS(0, matrix0_b.getValue(0, 0));
		TS_ASSERT_EQUALS(-4, matrix0_b.getValue(2, 2));
		TS_ASSERT_EQUALS(-6, matrix0_b.getValue(2, 4));

		TS_ASSERT_EQUALS(0, matrix1.getValue(0, 0));
		TS_ASSERT_EQUALS(4, matrix1.getValue(2, 2));
		TS_ASSERT_EQUALS(6, matrix1.getValue(2, 4));

		TS_ASSERT_EQUALS(true, matrix0 == matrix0);
		TS_ASSERT_EQUALS(true, matrix0 == matrix1);
		TS_ASSERT_EQUALS(true, matrix0 != matrix0_b);

		TS_ASSERT_EQUALS(false, matrix0 != matrix0);
		TS_ASSERT_EQUALS(false, matrix0 != matrix1);
		TS_ASSERT_EQUALS(false, matrix0 == matrix0_b);
	}

	void test_matrixBase_vectorMultiplication()
	{
		MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();
		VectorBase<int, 3> vector0;

		for(unsigned int i = 0; i < vector0.getDimensions(); i++)
		{
			vector0.setValue(i, i+1);
		}

		VectorBase<int, 5> vector0_r = multiply(matrix0, vector0);

		TS_ASSERT_EQUALS(8, vector0_r[0]);
		TS_ASSERT_EQUALS(14, vector0_r[1]);
		TS_ASSERT_EQUALS(20, vector0_r[2]);
		TS_ASSERT_EQUALS(26, vector0_r[3]);
		TS_ASSERT_EQUALS(32, vector0_r[4]);

		MatrixBase<int, 5, 3> matrix1 = getTestMatrix5x3();
		VectorBase<int, 3> vector1;

		for(unsigned int i = 0; i < vector1.getDimensions(); i++)
		{
			vector1.setValue(i, i+1);
		}

		VectorBase<int, 5> vector1_r = multiply(vector1, matrix1);

		TS_ASSERT_EQUALS(8, vector1_r[0]);
		TS_ASSERT_EQUALS(14, vector1_r[1]);
		TS_ASSERT_EQUALS(20, vector1_r[2]);
		TS_ASSERT_EQUALS(26, vector1_r[3]);
		TS_ASSERT_EQUALS(32, vector1_r[4]);

		MatrixBase<int, 3, 5> matrix2 = getTestMatrix3x5();
		VectorBase<int, 5> vector2;

		for(unsigned int i = 0; i < vector2.getDimensions(); i++)
		{
			vector2.setValue(i, i+1);
		}

		VectorBase<int, 3> vector2_r = multiply(vector2, matrix2);

		TS_ASSERT_EQUALS(40, vector2_r[0]);
		TS_ASSERT_EQUALS(55, vector2_r[1]);
		TS_ASSERT_EQUALS(70, vector2_r[2]);
	}

private:
	/**
	 * C++ functions can't return statically allocated arrays.
	 * I don't want to use dynamically allocated arrays, so here's my work around for that...
	 *
	 * Update: acutally they can... see MatrixBase [] operator (in MatrixBase.cpp)
	 */
	template<class T>
	struct Array3x5
	{
		T array[3][5];
	};

	template<class T>
	struct Array5x3
	{
		T array[5][3];
	};

	Array3x5<int> getTestValues3x5()
	{
		Array3x5<int> result;

		for(unsigned int i = 0; i < 3; i++)
		{
			for(unsigned int j = 0; j < 5; j++)
			{
				result.array[i][j] = i + j;
			}
		}

		return result;
	}

	Array3x5<int> getTestValues3x5_b()
	{
		Array3x5<int> result;

		for(unsigned int i = 0; i < 3; i++)
		{
			for(unsigned int j = 0; j < 5; j++)
			{
				result.array[i][j] = -i - j;
			}
		}

		return result;
	}

	Array5x3<int> getTestValues5x3()
	{
		Array5x3<int> result;

		for(unsigned int i = 0; i < 5; i++)
		{
			for(unsigned int j = 0; j < 3; j++)
			{
				result.array[i][j] = i + j;
			}
		}

		return result;
	}

	MatrixBase<int, 3, 5> getTestMatrix3x5()
	{
		Array3x5<int> testValues = getTestValues3x5();

		return MatrixBase<int, 3, 5>(testValues.array);
	}

	MatrixBase<int, 3, 5> getTestMatrix3x5_b()
	{
		Array3x5<int> testValues = getTestValues3x5_b();

		return MatrixBase<int, 3, 5>(testValues.array);
	}

	MatrixBase<int, 5, 3> getTestMatrix5x3()
	{
		Array5x3<int> testValues = getTestValues5x3();

		return MatrixBase<int, 5, 3>(testValues.array);
	}
};