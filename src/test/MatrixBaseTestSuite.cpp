#include "catch.hpp"

#include "MatrixBase.h"
#include "VectorBase.h"
namespace
{
/**
 * C++ functions can't return statically allocated arrays.
 * I don't want to use dynamically allocated arrays, so here's my work around for that...
 *
 * Update: acutally they can... see MatrixBase [] operator (in MatrixBase.cpp)
 */
template <class T>
struct Array3x5
{
	T array[3][5];
};

template <class T>
struct Array5x3
{
	T array[5][3];
};

Array3x5<int> getTestValues3x5()
{
	Array3x5<int> result;

	for (unsigned int i = 0; i < 3; i++)
	{
		for (unsigned int j = 0; j < 5; j++)
		{
			result.array[i][j] = i + j;
		}
	}

	return result;
}

Array3x5<int> getTestValues3x5_b()
{
	Array3x5<int> result;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			result.array[i][j] = -i - j;
		}
	}

	return result;
}

Array5x3<int> getTestValues5x3()
{
	Array5x3<int> result;

	for (unsigned int i = 0; i < 5; i++)
	{
		for (unsigned int j = 0; j < 3; j++)
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
}	 // namespace

TEST_CASE("matrixBase constructors")
{
	MatrixBase<int, 4, 5> matrix0;

	REQUIRE(4 == matrix0.getColumnsCount());
	REQUIRE(5 == matrix0.getRowsCount());

	Array3x5<int> testValues = getTestValues3x5();

	MatrixBase<int, 3, 5> matrix1(testValues.array);

	REQUIRE(3 == matrix1.getColumnsCount());
	REQUIRE(5 == matrix1.getRowsCount());

	REQUIRE(0 == matrix1.getValue(0, 0));
	REQUIRE(6 == matrix1.getValue(2, 4));

	MatrixBase<int, 3, 5> matrix2(matrix1);

	REQUIRE(0 == matrix2.getValue(0, 0));
	REQUIRE(6 == matrix2.getValue(2, 4));
}

TEST_CASE("matrixBase getSetValue")
{
	MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();

	int value2_2 = matrix0.getValue(2, 2);
	matrix0.setValue(2, 2, value2_2 * 2);
	REQUIRE(value2_2 * 2 == matrix0.getValue(2, 2));
	REQUIRE(3 == matrix0.getValue(1, 2));
	REQUIRE(0 == matrix0.getValue(0, 0));
}

TEST_CASE("matrixBase getRowsColumnsCount")
{
	MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();
	MatrixBase<int, 5, 3> matrix1 = getTestMatrix5x3();

	REQUIRE(3 == matrix0.getColumnsCount());
	REQUIRE(5 == matrix0.getRowsCount());

	REQUIRE(5 == matrix1.getColumnsCount());
	REQUIRE(3 == matrix1.getRowsCount());
}

TEST_CASE("matrixBase transposed")
{
	MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();
	REQUIRE(3 == matrix0.getColumnsCount());
	REQUIRE(5 == matrix0.getRowsCount());

	MatrixBase<int, 5, 3> matrix1 = matrix0.transposed();
	REQUIRE(5 == matrix1.getColumnsCount());
	REQUIRE(3 == matrix1.getRowsCount());

	REQUIRE(matrix0.getValue(0, 0) == matrix1.getValue(0, 0));
	REQUIRE(matrix0.getValue(0, 1) == matrix1.getValue(1, 0));
	REQUIRE(matrix0.getValue(0, 4) == matrix1.getValue(4, 0));
	REQUIRE(matrix0.getValue(1, 4) == matrix1.getValue(4, 1));
	REQUIRE(matrix0.getValue(2, 4) == matrix1.getValue(4, 2));
	REQUIRE(matrix0.getValue(0, 3) == matrix1.getValue(3, 0));
	REQUIRE(matrix0.getValue(1, 3) == matrix1.getValue(3, 1));
	REQUIRE(matrix0.getValue(2, 3) == matrix1.getValue(3, 2));
}

TEST_CASE("matrixBase assign")
{
	MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();
	MatrixBase<int, 3, 5> matrix1 = getTestMatrix3x5_b();

	REQUIRE(0 == matrix0.getValue(0, 0));
	REQUIRE(2 == matrix0.getValue(1, 1));
	REQUIRE(4 == matrix0.getValue(2, 2));

	REQUIRE(0 == matrix1.getValue(0, 0));
	REQUIRE(-2 == matrix1.getValue(1, 1));
	REQUIRE(-4 == matrix1.getValue(2, 2));

	matrix0.assign(matrix1);

	REQUIRE(0 == matrix0.getValue(0, 0));
	REQUIRE(-2 == matrix0.getValue(1, 1));
	REQUIRE(-4 == matrix0.getValue(2, 2));
}

TEST_CASE("matrixBase add subtract")
{
	MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();
	MatrixBase<int, 3, 5> matrix1 = getTestMatrix3x5_b();

	matrix0.add(matrix1);

	REQUIRE(0 == matrix0.getValue(0, 0));
	REQUIRE(0 == matrix0.getValue(1, 1));
	REQUIRE(0 == matrix0.getValue(2, 2));

	matrix0.subtract(matrix1);

	REQUIRE(0 == matrix0.getValue(0, 0));
	REQUIRE(2 == matrix0.getValue(1, 1));
	REQUIRE(4 == matrix0.getValue(2, 2));
}

TEST_CASE("matrixBase multiplyDivideScalar")
{
	MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();

#pragma warning(push)
#pragma warning(disable : 4244)
	matrix0.scalarMultiplication(2.0f);	   // float is on porpoise (so is porpoise, womp womp)

	REQUIRE(0 == matrix0.getValue(0, 0));
	REQUIRE(4 == matrix0.getValue(1, 1));
	REQUIRE(8 == matrix0.getValue(2, 2));

	matrix0.scalarMultiplication(0.5f);

	REQUIRE(0 == matrix0.getValue(0, 0));
	REQUIRE(2 == matrix0.getValue(1, 1));
	REQUIRE(4 == matrix0.getValue(2, 2));

	matrix0.scalarMultiplication(0.5f);

	REQUIRE(0 == matrix0.getValue(0, 0));
	REQUIRE(1 == matrix0.getValue(1, 1));
	REQUIRE(2 == matrix0.getValue(2, 2));

	matrix0.scalarMultiplication(0.5f);

	REQUIRE(0 == matrix0.getValue(0, 0));
	REQUIRE(0 == matrix0.getValue(1, 1));
	REQUIRE(1 == matrix0.getValue(2, 2));
#pragma warning(pop)
}

TEST_CASE("matrixBase multiplyMatrix")
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


	REQUIRE(5 == matrix2.getColumnsCount());
	REQUIRE(5 == matrix2.getRowsCount());

	REQUIRE(3 == matrix3.getColumnsCount());
	REQUIRE(3 == matrix3.getRowsCount());

	REQUIRE(-5 == matrix2.getValue(0, 0));
	REQUIRE(-77 == matrix2.getValue(4, 4));
	REQUIRE(-29 == matrix2.getValue(2, 2));
	REQUIRE(-11 == matrix2.getValue(2, 0));
	REQUIRE(-11 == matrix2.getValue(0, 2));
	REQUIRE(-38 == matrix2.getValue(3, 2));


	REQUIRE(-30 == matrix3.getValue(0, 0));
	REQUIRE(-90 == matrix3.getValue(2, 2));
	REQUIRE(-50 == matrix3.getValue(2, 0));
	REQUIRE(-50 == matrix3.getValue(0, 2));
	REQUIRE(-55 == matrix3.getValue(1, 1));
}

TEST_CASE("matrixBase isEqual")
{
	MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();
	MatrixBase<int, 3, 5> matrix0_b = getTestMatrix3x5();
	MatrixBase<int, 3, 5> matrix1 = getTestMatrix3x5_b();

	REQUIRE(true == matrix0.isEqual(matrix0_b));
	REQUIRE(false == matrix0.isEqual(matrix1));
	REQUIRE(true == matrix0.isEqual(matrix0));
}

TEST_CASE("matrixBase isSame")
{
	MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();
	MatrixBase<int, 3, 5> matrix0_b = getTestMatrix3x5();
	MatrixBase<int, 3, 5> matrix1 = getTestMatrix3x5_b();

	REQUIRE(false == matrix0.isSame(matrix0_b));
	REQUIRE(false == matrix0.isSame(matrix1));
	REQUIRE(true == matrix0.isSame(matrix0));
}

TEST_CASE("matrixBase accessOperator")
{
	MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();

	REQUIRE(0 == matrix0[0][0]);
	REQUIRE(4 == matrix0[2][2]);
	REQUIRE(6 == matrix0[2][4]);

	matrix0[0][0] = 42;

	REQUIRE(42 == matrix0[0][0]);
	REQUIRE(4 == matrix0[2][2]);
	REQUIRE(6 == matrix0[2][4]);
}

TEST_CASE("matrixBase operators")
{
	MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();
	MatrixBase<int, 3, 5> matrix0_b = getTestMatrix3x5_b();

	REQUIRE(0 == matrix0.getValue(0, 0));
	REQUIRE(4 == matrix0.getValue(2, 2));
	REQUIRE(6 == matrix0.getValue(2, 4));

	REQUIRE(0 == matrix0_b.getValue(0, 0));
	REQUIRE(-4 == matrix0_b.getValue(2, 2));
	REQUIRE(-6 == matrix0_b.getValue(2, 4));

	MatrixBase<int, 3, 5> matrix1 = matrix0 + matrix0_b;
	MatrixBase<int, 3, 5> matrix2 = matrix0 - matrix0_b;

	REQUIRE(0 == matrix1.getValue(0, 0));
	REQUIRE(0 == matrix1.getValue(2, 2));
	REQUIRE(0 == matrix1.getValue(2, 4));

	REQUIRE(0 == matrix2.getValue(0, 0));
	REQUIRE(8 == matrix2.getValue(2, 2));
	REQUIRE(12 == matrix2.getValue(2, 4));

	MatrixBase<int, 3, 5> matrix3 = matrix0 * 3;
	MatrixBase<int, 3, 5> matrix4 = matrix0 / 2;
	MatrixBase<int, 3, 5> matrix5 = matrix0 * 3.3f;	   // float is on purpose

	REQUIRE(0 == matrix3.getValue(0, 0));
	REQUIRE(12 == matrix3.getValue(2, 2));
	REQUIRE(18 == matrix3.getValue(2, 4));

	REQUIRE(0 == matrix4.getValue(0, 0));
	REQUIRE(2 == matrix4.getValue(2, 2));
	REQUIRE(3 == matrix4.getValue(2, 4));

	REQUIRE(0 == matrix5.getValue(0, 0));
	REQUIRE(13 == matrix5.getValue(2, 2));
	REQUIRE(19 == matrix5.getValue(2, 4));
}

TEST_CASE("matrixBase assignOperators")
{
	MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();
	MatrixBase<int, 3, 5> matrix0_b = getTestMatrix3x5_b();

	REQUIRE(0 == matrix0.getValue(0, 0));
	REQUIRE(4 == matrix0.getValue(2, 2));
	REQUIRE(6 == matrix0.getValue(2, 4));

	REQUIRE(0 == matrix0_b.getValue(0, 0));
	REQUIRE(-4 == matrix0_b.getValue(2, 2));
	REQUIRE(-6 == matrix0_b.getValue(2, 4));

	MatrixBase<int, 3, 5> matrix1 = getTestMatrix3x5();
	matrix1 += matrix0;

	REQUIRE(0 == matrix1.getValue(0, 0));
	REQUIRE(8 == matrix1.getValue(2, 2));
	REQUIRE(12 == matrix1.getValue(2, 4));

	matrix1 += matrix0_b;

	REQUIRE(0 == matrix1.getValue(0, 0));
	REQUIRE(4 == matrix1.getValue(2, 2));
	REQUIRE(6 == matrix1.getValue(2, 4));

	matrix1 -= matrix0_b;

	REQUIRE(0 == matrix1.getValue(0, 0));
	REQUIRE(8 == matrix1.getValue(2, 2));
	REQUIRE(12 == matrix1.getValue(2, 4));

	matrix1 *= 3.3f;

	REQUIRE(0 == matrix1.getValue(0, 0));
	REQUIRE(26 == matrix1.getValue(2, 2));
	REQUIRE(39 == matrix1.getValue(2, 4));

	matrix1 /= 3;

	REQUIRE(0 == matrix1.getValue(0, 0));
	REQUIRE(8 == matrix1.getValue(2, 2));
	REQUIRE(13 == matrix1.getValue(2, 4));
}

TEST_CASE("matrixBase comparisonOperators")
{
	MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();
	MatrixBase<int, 3, 5> matrix0_b = getTestMatrix3x5_b();
	MatrixBase<int, 3, 5> matrix1 = getTestMatrix3x5();

	REQUIRE(0 == matrix0.getValue(0, 0));
	REQUIRE(4 == matrix0.getValue(2, 2));
	REQUIRE(6 == matrix0.getValue(2, 4));

	REQUIRE(0 == matrix0_b.getValue(0, 0));
	REQUIRE(-4 == matrix0_b.getValue(2, 2));
	REQUIRE(-6 == matrix0_b.getValue(2, 4));

	REQUIRE(0 == matrix1.getValue(0, 0));
	REQUIRE(4 == matrix1.getValue(2, 2));
	REQUIRE(6 == matrix1.getValue(2, 4));

	REQUIRE(true == (matrix0 == matrix0));
	REQUIRE(true == (matrix0 == matrix1));
	REQUIRE(true == (matrix0 != matrix0_b));

	REQUIRE(false == (matrix0 != matrix0));
	REQUIRE(false == (matrix0 != matrix1));
	REQUIRE(false == (matrix0 == matrix0_b));
}

TEST_CASE("matrixBase vectorMultiplication")
{
	MatrixBase<int, 3, 5> matrix0 = getTestMatrix3x5();
	VectorBase<int, 3> vector0;

	for (unsigned int i = 0; i < vector0.getDimensions(); i++)
	{
		vector0.setValue(i, i + 1);
	}

	VectorBase<int, 5> vector0_r = multiply(matrix0, vector0);

	REQUIRE(8 == vector0_r[0]);
	REQUIRE(14 == vector0_r[1]);
	REQUIRE(20 == vector0_r[2]);
	REQUIRE(26 == vector0_r[3]);
	REQUIRE(32 == vector0_r[4]);

	MatrixBase<int, 5, 3> matrix1 = getTestMatrix5x3();
	VectorBase<int, 3> vector1;

	for (unsigned int i = 0; i < vector1.getDimensions(); i++)
	{
		vector1.setValue(i, i + 1);
	}

	VectorBase<int, 5> vector1_r = multiply(vector1, matrix1);

	REQUIRE(8 == vector1_r[0]);
	REQUIRE(14 == vector1_r[1]);
	REQUIRE(20 == vector1_r[2]);
	REQUIRE(26 == vector1_r[3]);
	REQUIRE(32 == vector1_r[4]);

	MatrixBase<int, 3, 5> matrix2 = getTestMatrix3x5();
	VectorBase<int, 5> vector2;

	for (unsigned int i = 0; i < vector2.getDimensions(); i++)
	{
		vector2.setValue(i, i + 1);
	}

	VectorBase<int, 3> vector2_r = multiply(vector2, matrix2);

	REQUIRE(40 == vector2_r[0]);
	REQUIRE(55 == vector2_r[1]);
	REQUIRE(70 == vector2_r[2]);
}
