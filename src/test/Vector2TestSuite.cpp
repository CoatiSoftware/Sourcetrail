#include "catch.hpp"

#include "Vector2.h"

TEST_CASE("vec2i constructors")
{
	Vec2i vec0;
	Vec2i vec1(1, 1);
	Vec2i vec2(vec1);

	REQUIRE(0 == vec0.x);
	REQUIRE(0 == vec0.y);

	REQUIRE(1 == vec1.x);
	REQUIRE(1 == vec1.y);

	REQUIRE(1 == vec2.x);
	REQUIRE(1 == vec2.y);
}

TEST_CASE("vec2f constructors")
{
	Vec2f vec0;
	Vec2f vec1(1.0f, 1.0f);
	Vec2f vec2(vec1);

	REQUIRE(0.0f == vec0.x);
	REQUIRE(0.0f == vec0.y);

	REQUIRE(1.0f == vec1.x);
	REQUIRE(1.0f == vec1.y);

	REQUIRE(1.0f == vec2.x);
	REQUIRE(1.0f == vec2.y);
}

TEST_CASE("vector2 get length")
{
	Vec2f vec0(3.0f, 3.0f);
	REQUIRE(18.0f == vec0.getLengthSquared());
	vec0.x = -3.0f;
	REQUIRE(18.0f == vec0.getLengthSquared());
	REQUIRE(std::sqrt(18.0f) == vec0.getLength());

	Vec2i vec1(4, 2);
	REQUIRE(20.0f == vec1.getLengthSquared());
	REQUIRE(std::sqrt(20.0f) == vec1.getLength());
}


TEST_CASE("vector2 normalization")
{
	Vec2f vec0(1.0f, 1.0f);

	float targetXY = 1.0f / std::sqrt(2.0f);

	REQUIRE(targetXY == Approx(vec0.normalized().x));
	REQUIRE(targetXY == Approx(vec0.normalized().y));

	vec0.normalize();
	REQUIRE(targetXY == Approx(vec0.x));
	REQUIRE(targetXY == Approx(vec0.y));

	float x = 17.53f;
	float y = 42.42f;
	vec0.x = x;
	vec0.y = y;
	float targetX = x / std::sqrt(x * x + y * y);
	float targetY = y / std::sqrt(x * x + y * y);
	vec0.normalize();
	REQUIRE(targetX == Approx(vec0.x));
	REQUIRE(targetY == Approx(vec0.y));
}

TEST_CASE("vector2 comparison")
{
	Vec2i vec0(42, 24);
	Vec2i vec1(42, 24);
	Vec2i vec2(69, 96);

	REQUIRE(true == vec0.isEqual(vec1));
	REQUIRE(false == vec0.isEqual(vec2));
	REQUIRE(true == vec0.isSame(vec0));
	REQUIRE(false == vec0.isSame(vec1));

	Vec2f vec3(42.0f, 24.0f);
	Vec2f vec4(42.0f, 24.0f);
	Vec2f vec5(69.0f, 96.0f);

	REQUIRE(true == vec3.isEqual(vec4));
	REQUIRE(false == vec3.isEqual(vec5));
	REQUIRE(true == vec3.isSame(vec3));
	REQUIRE(false == vec3.isSame(vec4));
}

TEST_CASE("vector2 comparison operators")
{
	Vec2i vec0(42, 24);
	Vec2i vec1(42, 24);
	Vec2i vec2(69, 96);

	REQUIRE(true == (vec0 == vec0));
	REQUIRE(true == (vec0 == vec1));
	REQUIRE(false == (vec0 == vec2));
	REQUIRE(false == (vec0 != vec0));
	REQUIRE(false == (vec0 != vec1));
	REQUIRE(true == (vec0 != vec2));

	Vec2f vec3(42.0f, 24.0f);
	Vec2f vec4(42.0f, 24.0f);
	Vec2f vec5(69.0f, 96.0f);

	REQUIRE(true == (vec3 == vec3));
	REQUIRE(true == (vec3 == vec4));
	REQUIRE(false == (vec3 == vec5));
	REQUIRE(false == (vec3 != vec3));
	REQUIRE(false == (vec3 != vec4));
	REQUIRE(true == (vec3 != vec5));
}

TEST_CASE("assignment operator")
{
	Vec2i vec0(42, 24);
	Vec2i vec1(69, 96);

	vec1 = vec0;
	REQUIRE(42 == vec1.x);
	REQUIRE(24 == vec1.y);
	REQUIRE(true == vec0.isEqual(vec1));
	REQUIRE(false == vec0.isSame(vec1));

	Vec2i vec2(42, 24);
	Vec2i vec3(69, 96);

	vec2 = vec3;
	REQUIRE(69 == vec2.x);
	REQUIRE(96 == vec2.y);
	REQUIRE(true == vec3.isEqual(vec2));
	REQUIRE(false == vec3.isSame(vec2));
}

TEST_CASE("addition operators")
{
	Vec2i vec0(-2, 2);
	Vec2i vec1(3, -3);
	Vec2i vec2 = vec0 + vec1;

	REQUIRE(1 == vec2.x);
	REQUIRE(-1 == vec2.y);

	REQUIRE(-2 == vec0.x);
	REQUIRE(2 == vec0.y);
	REQUIRE(3 == vec1.x);
	REQUIRE(-3 == vec1.y);

	vec0 += vec1;
	REQUIRE(1 == vec0.x);
	REQUIRE(-1 == vec0.y);
}

TEST_CASE("subtraction operators")
{
	Vec2f vec0(-2.0f, 2.0f);
	Vec2f vec1(3.0f, -3.0f);
	Vec2f vec2 = vec0 - vec1;

	REQUIRE(-5.0f == vec2.x);
	REQUIRE(5.0f == vec2.y);

	REQUIRE(-2.0f == vec0.x);
	REQUIRE(2.0f == vec0.y);
	REQUIRE(3.0f == vec1.x);
	REQUIRE(-3.0f == vec1.y);

	vec0 -= vec1;
	REQUIRE(-5.0f == vec0.x);
	REQUIRE(5.0f == vec0.y);
}

TEST_CASE("scalar multiplication operators")
{
	Vec2f vec0(-2.0f, 2.0f);
	Vec2f vec1 = vec0 * 42.0f;

	REQUIRE(-84.0f == vec1.x);
	REQUIRE(84.0f == vec1.y);

	REQUIRE(-2.0f == vec0.x);
	REQUIRE(2.0f == vec0.y);

	vec0 *= 42.0f;

	REQUIRE(-84.0f == vec0.x);
	REQUIRE(84.0f == vec0.y);

#pragma warning(push)
#pragma warning(disable : 4244)
	Vec2i vec2(-2, 2);
	Vec2i vec3 = vec2 * 42.4f;
#pragma warning(pop)

	REQUIRE(-84 == (int)vec3.x);
	REQUIRE(84 == (int)vec3.y);

	Vec2i vec3b = vec2 * 42.5f;

	REQUIRE(-85 == (int)vec3b.x);
	REQUIRE(85 == (int)vec3b.y);

	REQUIRE(-2 == vec2.x);
	REQUIRE(2 == vec2.y);

	vec2 *= 42;
	REQUIRE(-84 == vec2.x);
	REQUIRE(84 == vec2.y);

	vec2 *= 0.5f;
	REQUIRE(-42 == vec2.x);
	REQUIRE(42 == vec2.y);
}

TEST_CASE("dot product operator")
{
	Vec2f vec0(2.0f, 4.0f);
	Vec2f vec1(3.0f, 6.0f);
	Vec2f vec2(-2.0f, -1.0f);

	REQUIRE(30.0f == vec0.dotProduct(vec1));
	REQUIRE(-8.0f == vec0.dotProduct(vec2));

	Vec2f vec3(2, 4);
	Vec2f vec4(3, 6);
	Vec2f vec5(-2, -1);

	REQUIRE(30 == vec3.dotProduct(vec4));
	REQUIRE(-8 == vec3.dotProduct(vec5));
}

TEST_CASE("scalar division operators")
{
	Vec2f vec0(42.0f, 24.0f);
	Vec2f vec1 = vec0 / 2.0f;
	Vec2f vec2 = vec0 / 0.5f;

	REQUIRE(42.0f == vec0.x);
	REQUIRE(24.0f == vec0.y);
	REQUIRE(21.0f == vec1.x);
	REQUIRE(12.0f == vec1.y);
	REQUIRE(84.0f == vec2.x);
	REQUIRE(48.0f == vec2.y);

	vec0 /= 2.0f;
	REQUIRE(21.0f == vec0.x);
	REQUIRE(12.0f == vec0.y);

	Vec2i vec3(42, 24);
	Vec2i vec4 = vec3 / 2;

	REQUIRE(42 == vec3.x);
	REQUIRE(24 == vec3.y);
	REQUIRE(21 == vec4.x);
	REQUIRE(12 == vec4.y);

	vec3 /= 2;
	REQUIRE(21 == vec3.x);
	REQUIRE(12 == vec3.y);
}
