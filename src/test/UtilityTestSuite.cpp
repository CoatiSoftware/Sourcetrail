#include <catch2/catch.hpp>

#include "utility.h"

TEST_CASE("trim blank spaces of string")
{
	REQUIRE(utility::trim(" foo  ") == "foo");
}

TEST_CASE("trim blank spaces of wstring")
{
	REQUIRE(utility::trim(L" foo  ") == L"foo");
}
