#include "catch.hpp"

#include "utilityString.h"

TEST_CASE("split with char delimiter")
{
	std::deque<std::string> result = utility::split("A,B,C", ',');

	REQUIRE(result.size() == 3);
	REQUIRE(result.at(0) == "A");
	REQUIRE(result.at(1) == "B");
	REQUIRE(result.at(2) == "C");
}

TEST_CASE("split with string delimiter")
{
	std::deque<std::string> result = utility::split("A->B>C", "->");

	REQUIRE(result.size() == 2);
	REQUIRE(result.at(0) == "A");
	REQUIRE(result.at(1) == "B>C");
}

TEST_CASE("split on empty string")
{
	std::deque<std::string> result = utility::split("", "->");

	REQUIRE(result.size() == 1);
	REQUIRE(result.at(0) == "");
}

TEST_CASE("split with unused delimiter")
{
	std::deque<std::string> result = utility::split("A:B:C", ";");

	REQUIRE(result.size() == 1);
	REQUIRE(result.at(0) == "A:B:C");
}

TEST_CASE("split with delimiters next to each")
{
	std::deque<std::string> result = utility::split("A::B:C", ':');

	REQUIRE(result.size() == 4);
	REQUIRE(result.at(0) == "A");
	REQUIRE(result.at(1) == "");
	REQUIRE(result.at(2) == "B");
	REQUIRE(result.at(3) == "C");
}

TEST_CASE("split with delimiter at start")
{
	std::deque<std::string> result = utility::split(":B:C", ':');

	REQUIRE(result.size() == 3);
	REQUIRE(result.at(0) == "");
	REQUIRE(result.at(1) == "B");
	REQUIRE(result.at(2) == "C");
}

TEST_CASE("split with delimiter at end")
{
	std::deque<std::string> result = utility::split("B:C:", ':');

	REQUIRE(result.size() == 3);
	REQUIRE(result.at(0) == "B");
	REQUIRE(result.at(1) == "C");
	REQUIRE(result.at(2) == "");
}

TEST_CASE("join with char delimiter")
{
	std::deque<std::string> list;
	list.push_back("A");
	list.push_back("B");
	list.push_back("C");

	std::string result = utility::join(list, ',');
	REQUIRE(result == "A,B,C");
}

TEST_CASE("join with string delimiter")
{
	std::deque<std::string> list;
	list.push_back("A");
	list.push_back("B");
	list.push_back("C");

	std::string result = utility::join(list, "==");
	REQUIRE(result == "A==B==C");
}

TEST_CASE("join on empty list")
{
	std::deque<std::string> list;
	std::string result = utility::join(list, ',');
	REQUIRE(result == "");
}

TEST_CASE("join with empty strings in list")
{
	std::deque<std::string> list;
	list.push_back("A");
	list.push_back("");
	list.push_back("");

	std::string result = utility::join(list, ':');
	REQUIRE(result == "A::");
}

TEST_CASE("tokenize with string")
{
	std::deque<std::string> result = utility::tokenize("A->B->C", "->");

	REQUIRE(result.size() == 5);
	REQUIRE(result.at(0) == "A");
	REQUIRE(result.at(1) == "->");
	REQUIRE(result.at(2) == "B");
	REQUIRE(result.at(3) == "->");
	REQUIRE(result.at(4) == "C");
}

TEST_CASE("tokenize with string and delimiter at start")
{
	std::deque<std::string> result = utility::tokenize("->B", "->");

	REQUIRE(result.size() == 2);
	REQUIRE(result.at(0) == "->");
	REQUIRE(result.at(1) == "B");
}

TEST_CASE("tokenize with string and delimiter at end")
{
	std::deque<std::string> result = utility::tokenize("C+", '+');

	REQUIRE(result.size() == 2);
	REQUIRE(result.at(0) == "C");
	REQUIRE(result.at(1) == "+");
}

TEST_CASE("tokenize with deque")
{
	std::deque<std::string> result = utility::tokenize("A->B=C->D", "->");
	result = utility::tokenize(result, "=");

	REQUIRE(result.size() == 7);
	REQUIRE(result.at(0) == "A");
	REQUIRE(result.at(1) == "->");
	REQUIRE(result.at(2) == "B");
	REQUIRE(result.at(3) == "=");
	REQUIRE(result.at(4) == "C");
	REQUIRE(result.at(5) == "->");
	REQUIRE(result.at(6) == "D");
}

TEST_CASE("substr before first with single delimiter occurence")
{
	REQUIRE(utility::substrBeforeFirst("foo bar", ' ') == "foo");
}

TEST_CASE("substr before first with multiple delimiter occurences")
{
	REQUIRE(utility::substrBeforeFirst("foo bar foo", ' ') == "foo");
}

TEST_CASE("substr before first with no delimiter occurence")
{
	REQUIRE(utility::substrBeforeFirst("foobar", ' ') == "foobar");
}

TEST_CASE("substr before first with delimiter at start")
{
	REQUIRE(utility::substrBeforeFirst(" foobar", ' ') == "");
}

TEST_CASE("substr before first with delimiter at end")
{
	REQUIRE(utility::substrBeforeFirst("foobar ", ' ') == "foobar");
}

TEST_CASE("substr before last with single delimiter occurence")
{
	REQUIRE(utility::substrBeforeLast("foo bar", ' ') == "foo");
}

TEST_CASE("substr before last with multiple delimiter occurences")
{
	REQUIRE(utility::substrBeforeLast("foo bar foo", ' ') == "foo bar");
}

TEST_CASE("substr before last with no delimiter occurence")
{
	REQUIRE(utility::substrBeforeLast("foobar", ' ') == "foobar");
}

TEST_CASE("substr before last with delimiter at start")
{
	REQUIRE(utility::substrBeforeLast(" foobar", ' ') == "");
}

TEST_CASE("substr before last with delimiter at end")
{
	REQUIRE(utility::substrBeforeLast("foobar ", ' ') == "foobar");
}

TEST_CASE("substr after with single delimiter occurence")
{
	REQUIRE(utility::substrAfter("foo bar", ' ') == "bar");
}

TEST_CASE("substr after with multiple delimiter occurences")
{
	REQUIRE(utility::substrAfter("foo bar foo", ' ') == "bar foo");
}

TEST_CASE("substr after with no delimiter occurence")
{
	REQUIRE(utility::substrAfter("foobar", ' ') == "foobar");
}

TEST_CASE("substr after with delimiter at start")
{
	REQUIRE(utility::substrAfter(" foobar", ' ') == "foobar");
}

TEST_CASE("substr after with delimiter at end")
{
	REQUIRE(utility::substrAfter("foobar ", ' ') == "");
}

TEST_CASE("empty string is detected as prefix of any other string")
{
	const std::string foo = "foo";

	REQUIRE(utility::isPrefix<std::string>("", foo));
}

TEST_CASE("prefix of bigger text is detected as prefix")
{
	const std::string foobar = "foobar";
	const std::string foo = "foo";

	REQUIRE(utility::isPrefix(foo, foobar));
}

TEST_CASE("prefix is detected as prefix of self")
{
	const std::string foo = "foo";

	REQUIRE(utility::isPrefix(foo, foo));
}

TEST_CASE("different texts are not detected of prefixes of each other")
{
	const std::string foo = "foo";
	const std::string bar = "bar";

	REQUIRE(!utility::isPrefix(foo, bar));
	REQUIRE(!utility::isPrefix(bar, foo));
}

TEST_CASE("to lower case")
{
	REQUIRE("foobar" == utility::toLowerCase("FooBar"));
	REQUIRE("foobar" == utility::toLowerCase("FOOBAR"));
	REQUIRE("foobar" == utility::toLowerCase("foobar"));
}

TEST_CASE("equals case insensitive with different cases")
{
	const std::string foo = "FooBar";
	const std::string foo2 = "foobar";

	REQUIRE(utility::equalsCaseInsensitive(foo, foo2));
}

TEST_CASE("equals case insensitive with same cases")
{
	const std::string foo = "foobar";
	const std::string foo2 = "foobar";

	REQUIRE(utility::equalsCaseInsensitive(foo, foo2));
}

TEST_CASE("equals case insensitive with different strings")
{
	const std::string foo = "foo";
	const std::string foo2 = "foobar";

	REQUIRE(!utility::equalsCaseInsensitive(foo, foo2));
}

TEST_CASE("replace")
{
	REQUIRE("fubar" == utility::replace("foobar", "oo", "u"));
	REQUIRE("fuuuubar" == utility::replace("foobar", "o", "uu"));
	REQUIRE("bar" == utility::replace("foobar", "foo", ""));
	REQUIRE("foobar" == utility::replace("foobar", "", "i"));
	REQUIRE("foobar" == utility::replace("foobar", "", ""));
	REQUIRE("" == utility::replace("", "foo", "bar"));
	REQUIRE("foobar" == utility::replace("foobar", "ba", "ba"));
}

TEST_CASE("caseInsensitiveLess should return false when comparing empty wstrings")
{
	REQUIRE_FALSE(utility::caseInsensitiveLess(L"", L""));
}

TEST_CASE("caseInsensitiveLess should return false when both wstrings are equal")
{
	REQUIRE_FALSE(utility::caseInsensitiveLess(L"ab_cd!", L"ab_cd!"));
}

TEST_CASE(
	"caseInsensitiveLess should return false when both wstrings have"
	"different cases but after lower casing are equal")
{
	REQUIRE_FALSE(utility::caseInsensitiveLess(L"ab_CD!", L"aB_cD!"));
}

TEST_CASE("caseInsensitiveLess should return true when first wstring is empty and second not")
{
	REQUIRE(utility::caseInsensitiveLess(L"", L"ab"));
}

TEST_CASE("caseInsensitiveLess should return false when second wstring is empty and first not")
{
	REQUIRE_FALSE(utility::caseInsensitiveLess(L"ab", L""));
}

TEST_CASE("caseInsensitiveLess should return true when first wstring is prefix of second")
{
	REQUIRE(utility::caseInsensitiveLess(L"ab_cd!", L"ab_cd!e"));
}

TEST_CASE("caseInsensitiveLess should return false when second wstring is prefix of first")
{
	REQUIRE_FALSE(utility::caseInsensitiveLess(L"ab_cd!e", L"ab_cd!"));
}

TEST_CASE(
	"caseInsensitiveLess should return true when after lower casing first wstring, first is prefix "
	"of second")
{
	REQUIRE(utility::caseInsensitiveLess(L"aB_cd!", L"ab_cd!e"));
}

TEST_CASE(
	"caseInsensitiveLess should return true when after lower casing second wstring, first is "
	"prefix of second")
{
	REQUIRE(utility::caseInsensitiveLess(L"ab_cd!", L"ab_cD!e"));
}

TEST_CASE(
	"caseInsensitiveLess should return true when after lower casing both wstrings, first is prefix "
	"of second")
{
	REQUIRE(utility::caseInsensitiveLess(L"aB_cd!", L"ab_cD!E"));
}

TEST_CASE(
	"caseInsensitiveLess should return false when after lower casing first wstring, second is "
	"prefix of first")
{
	REQUIRE_FALSE(utility::caseInsensitiveLess(L"ab_Cd!e", L"ab_cd!"));
}

TEST_CASE(
	"caseInsensitiveLess should return false when after lower casing second wstring, second is "
	"prefix of first")
{
	REQUIRE_FALSE(utility::caseInsensitiveLess(L"ab_cd!e", L"Ab_cd!"));
}

TEST_CASE(
	"caseInsensitiveLess should return false when after lower casing both wstrings, second is "
	"prefix of first")
{
	REQUIRE_FALSE(utility::caseInsensitiveLess(L"ab_cD!E", L"aB_cd!"));
}
