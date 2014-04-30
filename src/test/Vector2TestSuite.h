#include "cxxtest/TestSuite.h"

#include "utility/math/Vector2.h"

class Vector2TestSuite : public CxxTest::TestSuite
{
public:
	void test_vec2i_constructors()
	{
		Vec2i vec0;
		Vec2i vec1(1, 1);
		Vec2i vec2(vec1);

		TS_ASSERT_EQUALS(0, vec0.x);
		TS_ASSERT_EQUALS(0, vec0.y);

		TS_ASSERT_EQUALS(1, vec1.x);
		TS_ASSERT_EQUALS(1, vec1.y);

		TS_ASSERT_EQUALS(1, vec2.x);
		TS_ASSERT_EQUALS(1, vec2.y);
	}

	void test_vec2f_constructors()
	{
		Vec2f vec0;
		Vec2f vec1(1.0f, 1.0f);
		Vec2f vec2(vec1);

		TS_ASSERT_EQUALS(0.0f, vec0.x);
		TS_ASSERT_EQUALS(0.0f, vec0.y);

		TS_ASSERT_EQUALS(1.0f, vec1.x);
		TS_ASSERT_EQUALS(1.0f, vec1.y);

		TS_ASSERT_EQUALS(1.0f, vec2.x);
		TS_ASSERT_EQUALS(1.0f, vec2.y);
	}

	void test_vector2_get_length()
	{
		Vec2f vec0(3.0f, 3.0f);
		TS_ASSERT_EQUALS(18.0f, vec0.getLengthSquared());
		vec0.x = -3.0f;
		TS_ASSERT_EQUALS(18.0f, vec0.getLengthSquared());
		TS_ASSERT_EQUALS(std::sqrt(18.0f), vec0.getLength());

		Vec2i vec1(4, 2);
		TS_ASSERT_EQUALS(20.0f, vec1.getLengthSquared());
		TS_ASSERT_EQUALS(std::sqrt(20.0f), vec1.getLength());
	}


	void test_vector2_normalization()
	{
		Vec2f vec0(1.0f, 1.0f);

		float targetXY = 1.0f / std::sqrt(2.0f);

		TS_ASSERT_EQUALS(targetXY, vec0.normalized().x);
		TS_ASSERT_EQUALS(targetXY, vec0.normalized().y);

		vec0.normalize();
		TS_ASSERT_EQUALS(targetXY, vec0.x);
		TS_ASSERT_EQUALS(targetXY, vec0.y);

		float x = 17.53f;
		float y = 42.42f;
		vec0.x = x;
		vec0.y = y;
		float targetX = x / std::sqrt(x * x + y * y);
		float targetY = y / std::sqrt(x * x + y * y);
		vec0.normalize();
		TS_ASSERT_EQUALS(targetX, vec0.x);
		TS_ASSERT_EQUALS(targetY, vec0.y);
	}

	void test_vector2_comparison()
	{
		Vec2i vec0(42, 24);
		Vec2i vec1(42, 24);
		Vec2i vec2(69, 96);

		TS_ASSERT_EQUALS(true, vec0.isEqual(vec1));
		TS_ASSERT_EQUALS(false, vec0.isEqual(vec2));
		TS_ASSERT_EQUALS(true, vec0.isSame(vec0));
		TS_ASSERT_EQUALS(false, vec0.isSame(vec1));

		Vec2f vec3(42.0f, 24.0f);
		Vec2f vec4(42.0f, 24.0f);
		Vec2f vec5(69.0f, 96.0f);

		TS_ASSERT_EQUALS(true, vec3.isEqual(vec4));
		TS_ASSERT_EQUALS(false, vec3.isEqual(vec5));
		TS_ASSERT_EQUALS(true, vec3.isSame(vec3));
		TS_ASSERT_EQUALS(false, vec3.isSame(vec4));
	}

	void test_vector2_comparison_operators()
	{
		Vec2i vec0(42, 24);
		Vec2i vec1(42, 24);
		Vec2i vec2(69, 96);

		TS_ASSERT_EQUALS(true, vec0 == vec0);
		TS_ASSERT_EQUALS(true, vec0 == vec1);
		TS_ASSERT_EQUALS(false, vec0 == vec2);
		TS_ASSERT_EQUALS(false, vec0 != vec0);
		TS_ASSERT_EQUALS(false, vec0 != vec1);
		TS_ASSERT_EQUALS(true, vec0 != vec2);

		Vec2f vec3(42.0f, 24.0f);
		Vec2f vec4(42.0f, 24.0f);
		Vec2f vec5(69.0f, 96.0f);

		TS_ASSERT_EQUALS(true, vec3 == vec3);
		TS_ASSERT_EQUALS(true, vec3 == vec4);
		TS_ASSERT_EQUALS(false, vec3 == vec5);
		TS_ASSERT_EQUALS(false, vec3 != vec3);
		TS_ASSERT_EQUALS(false, vec3 != vec4);
		TS_ASSERT_EQUALS(true, vec3 != vec5);
	}

	void test_assignment_operator()
	{
		Vec2i vec0(42, 24);
		Vec2i vec1(69, 96);

		vec1 = vec0;
		TS_ASSERT_EQUALS(42, vec1.x);
		TS_ASSERT_EQUALS(24, vec1.y);
		TS_ASSERT_EQUALS(true, vec0.isEqual(vec1));
		TS_ASSERT_EQUALS(false, vec0.isSame(vec1));

		Vec2i vec2(42, 24);
		Vec2i vec3(69, 96);

		vec2 = vec3;
		TS_ASSERT_EQUALS(69, vec2.x);
		TS_ASSERT_EQUALS(96, vec2.y);
		TS_ASSERT_EQUALS(true, vec3.isEqual(vec2));
		TS_ASSERT_EQUALS(false, vec3.isSame(vec2));
	}

	void test_addition_operators()
	{
		Vec2i vec0(-2, 2);
		Vec2i vec1(3, -3);
		Vec2i vec2 = vec0 + vec1;

		TS_ASSERT_EQUALS(1, vec2.x);
		TS_ASSERT_EQUALS(-1, vec2.y);

		TS_ASSERT_EQUALS(-2, vec0.x);
		TS_ASSERT_EQUALS(2, vec0.y);
		TS_ASSERT_EQUALS(3, vec1.x);
		TS_ASSERT_EQUALS(-3, vec1.y);

		vec0 += vec1;
		TS_ASSERT_EQUALS(1, vec0.x);
		TS_ASSERT_EQUALS(-1, vec0.y);
	}

	void test_subtraction_operators()
	{
		Vec2f vec0(-2.0f, 2.0f);
		Vec2f vec1(3.0f, -3.0f);
		Vec2f vec2 = vec0 - vec1;

		TS_ASSERT_EQUALS(-5.0f, vec2.x);
		TS_ASSERT_EQUALS(5.0f, vec2.y);

		TS_ASSERT_EQUALS(-2.0f, vec0.x);
		TS_ASSERT_EQUALS(2.0f, vec0.y);
		TS_ASSERT_EQUALS(3.0f, vec1.x);
		TS_ASSERT_EQUALS(-3.0f, vec1.y);

		vec0 -= vec1;
		TS_ASSERT_EQUALS(-5.0f, vec0.x);
		TS_ASSERT_EQUALS(5.0f, vec0.y);
	}

	void test_scalar_multiplication_operators()
	{
		Vec2f vec0(-2.0f, 2.0f);
		Vec2f vec1 = vec0 * 42.0f;

		TS_ASSERT_EQUALS(-84.0f, vec1.x);
		TS_ASSERT_EQUALS(84.0f, vec1.y);

		TS_ASSERT_EQUALS(-2.0f, vec0.x);
		TS_ASSERT_EQUALS(2.0f, vec0.y);

		vec0 *= 42.0f;

		TS_ASSERT_EQUALS(-84.0f, vec0.x);
		TS_ASSERT_EQUALS(84.0f, vec0.y);

		Vec2i vec2(-2, 2);
		Vec2i vec3 = vec2 * float(42.5f);

		TS_ASSERT_EQUALS(-84, vec3.x);
		TS_ASSERT_EQUALS(84, vec3.y);

		TS_ASSERT_EQUALS(-2, vec2.x);
		TS_ASSERT_EQUALS(2, vec2.y);

		vec2 *= 42;
		TS_ASSERT_EQUALS(-84, vec2.x);
		TS_ASSERT_EQUALS(84, vec2.y);
	}

	void test_dot_product_operator()
	{
		Vec2f vec0(2.0f, 4.0f);
		Vec2f vec1(3.0f, 6.0f);
		Vec2f vec2(-2.0f, -1.0f);

		TS_ASSERT_EQUALS(30.0f, vec0*vec1);
		TS_ASSERT_EQUALS(-8.0f, vec0*vec2);

		Vec2f vec3(2, 4);
		Vec2f vec4(3, 6);
		Vec2f vec5(-2, -1);

		TS_ASSERT_EQUALS(30, vec3 * vec4);
		TS_ASSERT_EQUALS(-8, vec3 * vec5);
	}

	void test_scalar_division_operators()
	{
		Vec2f vec0(42.0f, 24.0f);
		Vec2f vec1 = vec0 / 2.0f;
		Vec2f vec2 = vec0 / 0.5f;

		TS_ASSERT_EQUALS(42.0f, vec0.x);
		TS_ASSERT_EQUALS(24.0f, vec0.y);
		TS_ASSERT_EQUALS(21.0f, vec1.x);
		TS_ASSERT_EQUALS(12.0f, vec1.y);
		TS_ASSERT_EQUALS(84.0f, vec2.x);
		TS_ASSERT_EQUALS(48.0f, vec2.y);

		vec0 /= 2.0f;
		TS_ASSERT_EQUALS(21.0f, vec0.x);
		TS_ASSERT_EQUALS(12.0f, vec0.y);

		Vec2i vec3(42, 24);
		Vec2i vec4 = vec3 / 2;

		TS_ASSERT_EQUALS(42, vec3.x);
		TS_ASSERT_EQUALS(24, vec3.y);
		TS_ASSERT_EQUALS(21, vec4.x);
		TS_ASSERT_EQUALS(12, vec4.y);

		vec3 /= 2;
		TS_ASSERT_EQUALS(21, vec3.x);
		TS_ASSERT_EQUALS(12, vec3.y);
	}
};
