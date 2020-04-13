#include "catch.hpp"

#include "../lib/utility/utility.h"

TEST_CASE("trim blank spaces of string")
{
	REQUIRE(utility::trim(" foo  ") == "foo");
}

TEST_CASE("trim blank spaces of wstring")
{
	REQUIRE(utility::trim(L" foo  ") == L"foo");
}
