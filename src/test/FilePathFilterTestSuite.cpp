#include "catch.hpp"

#include "../lib/utility/file/FilePathFilter.h"

TEST_CASE("file path filter finds exact match")
{
	FilePathFilter filter(L"test.h");

	REQUIRE(filter.isMatching(FilePath(L"test.h")));
}

TEST_CASE("file path filter finds match with single asterisk in same level")
{
	FilePathFilter filter(L"*test.*");

	REQUIRE(filter.isMatching(FilePath(L"this_is_a_test.h")));
}

TEST_CASE("file path filter finds match with single asterisk in different level")
{
	FilePathFilter filter(L"*/this_is_a_test.h");

	REQUIRE(filter.isMatching(FilePath(L"folder/this_is_a_test.h")));
}

TEST_CASE("file path filter does not find match with single asterisk in different level")
{
	FilePathFilter filter(L"*/test.h");

	REQUIRE(!filter.isMatching(FilePath(L"test.h")));
}

TEST_CASE("file path filter finds match with multiple asterisk in same level")
{
	FilePathFilter filter(L"**test.h");

	REQUIRE(filter.isMatching(FilePath(L"folder/this_is_a_test.h")));
}

TEST_CASE("file path filter finds match with multiple asterisk in different level")
{
	FilePathFilter filter(L"root/**/test.h");

	REQUIRE(filter.isMatching(FilePath(L"root/folder1/folder2/test.h")));
}

TEST_CASE("file path filter does not find match with multiple asterisk in different level")
{
	FilePathFilter filter(L"**/test.h");

	REQUIRE(!filter.isMatching(FilePath(L"folder/this_is_a_test.h")));
}

TEST_CASE("file path filter escapes dot character")
{
	FilePathFilter filter(L"test.h");

	REQUIRE(!filter.isMatching(FilePath(L"testyh")));
}

TEST_CASE("file path filter escapes plus character")
{
	REQUIRE(FilePathFilter(L"folder/test+.h").isMatching(FilePath(L"folder/test+.h")));
}

TEST_CASE("file path filter escapes minus character")
{
	REQUIRE(FilePathFilter(L"folder/test[-].h").isMatching(FilePath(L"folder/test[-].h")));
}

TEST_CASE("file path filter escapes dollar character")
{
	REQUIRE(FilePathFilter(L"folder/test$.h").isMatching(FilePath(L"folder/test$.h")));
}

TEST_CASE("file path filter escapes circumflex character")
{
	REQUIRE(FilePathFilter(L"folder/test^.h").isMatching(FilePath(L"folder/test^.h")));
}

TEST_CASE("file path filter escapes open round brace character")
{
	REQUIRE(FilePathFilter(L"folder/test(.h").isMatching(FilePath(L"folder/test(.h")));
}

TEST_CASE("file path filter escapes close round brace character")
{
	REQUIRE(FilePathFilter(L"folder\\test).h").isMatching(FilePath(L"folder/test).h")));
}

TEST_CASE("file path filter escapes open curly brace character")
{
	REQUIRE(FilePathFilter(L"folder/test{.h").isMatching(FilePath(L"folder/test{.h")));
}

TEST_CASE("file path filter escapes close curly brace character")
{
	REQUIRE(FilePathFilter(L"folder/test}.h").isMatching(FilePath(L"folder/test}.h")));
}

TEST_CASE("file path filter escapes open squared brace character")
{
	REQUIRE(FilePathFilter(L"folder/test[.h").isMatching(FilePath(L"folder/test[.h")));
}

TEST_CASE("file path filter escapes close squared brace character")
{
	REQUIRE(FilePathFilter(L"folder\\test].h").isMatching(FilePath(L"folder/test].h")));
}

TEST_CASE("file path filter finds backslash if slash was provided")
{
	FilePathFilter filter(L"folder/test.h");

	REQUIRE(filter.isMatching(FilePath(L"folder\\test.h")));
}

TEST_CASE("file path filter finds slash if backslash was provided")
{
	FilePathFilter filter(L"folder\\test.h");

	REQUIRE(filter.isMatching(FilePath(L"folder/test.h")));
}
