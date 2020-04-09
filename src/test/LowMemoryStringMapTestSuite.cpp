#include <catch2/catch.hpp>

#include "LowMemoryStringMap.h"
#include "TextAccess.h"
#include "types.h"
#include "utilityString.h"

TEST_CASE("roughly everything")
{
	LowMemoryStringMap<std::string, Id, 0> map;

	map.add("abcdefg", 2);
	map.add("abcdefgerlitz", 1);
	map.add("abcdefghij", 3);
	map.add("abc", 4);

	// map.print(std::cout);
	// std::cout << std::endl << map.getByteSize() << " : " << map.getUncompressedByteSize() << std::endl;

	REQUIRE(map.find("abcdefgerlitz") == 1);
	REQUIRE(map.find("abcdefg") == 2);
	REQUIRE(map.find("abcdefghij") == 3);
	REQUIRE(map.find("abc") == 4);
	REQUIRE(map.find("bc") == 0);
	REQUIRE(map.find("") == 0);
	REQUIRE(map.find(";asdfl;kjasd;flkasdf") == 0);

	// TS_ASSERT(map.getByteSize() < map.getUncompressedByteSize());
}

TEST_CASE("cannot find element after creation")
{
	LowMemoryStringMap<std::string, Id, 0> map;

	REQUIRE(map.find("a") == 0);
}

TEST_CASE("find element")
{
	LowMemoryStringMap<std::string, Id, 0> map;
	map.add("a", 1);

	REQUIRE(map.find("a") == 1);
	REQUIRE(map.find("b") == 0);
}

TEST_CASE("find fully different elements")
{
	LowMemoryStringMap<std::string, Id, 0> map;
	map.add("a", 1);
	map.add("b", 2);

	REQUIRE(map.find("a") == 1);
	REQUIRE(map.find("b") == 2);
}

TEST_CASE("find similar short elements")
{
	LowMemoryStringMap<std::string, Id, 0> map;
	map.add("ab", 1);
	map.add("ac", 2);

	REQUIRE(map.find("ab") == 1);
	REQUIRE(map.find("ac") == 2);
	REQUIRE(map.find("bc") == 0);
}

TEST_CASE("find similar long elements")
{
	LowMemoryStringMap<std::string, Id, 0> map;
	map.add("aaaaabbbbb", 1);
	map.add("aaaaaccccc", 2);
	map.add("aaaaccccc", 3);
	map.add("aaaccccc", 4);

	REQUIRE(map.find("aaaaabbbbb") == 1);
	REQUIRE(map.find("aaaaaccccc") == 2);
	REQUIRE(map.find("aaaaacccccc") == 0);
	REQUIRE(map.find("aacc") == 0);
}

TEST_CASE("add twice")
{
	LowMemoryStringMap<std::string, Id, 0> map;
	map.add("abba", 1);
	map.add("abba", 2);

	REQUIRE(map.find("abba") == 1);
}

TEST_CASE("find parent child elements")
{
	LowMemoryStringMap<std::string, Id, 0> map;
	map.add("a", 1);
	map.add("ab", 2);

	REQUIRE(map.find("a") == 1);
	REQUIRE(map.find("ab") == 2);
	REQUIRE(map.find("b") == 0);
}

TEST_CASE("find long parent child elements")
{
	LowMemoryStringMap<std::string, Id, 0> map;
	map.add("ababaaa", 1);
	map.add("aba", 2);

	REQUIRE(map.find("ababaaa") == 1);
	REQUIRE(map.find("aba") == 2);
	REQUIRE(map.find("ab") == 0);
	REQUIRE(map.find("") == 0);
}

TEST_CASE("find long similar prefix elements")
{
	LowMemoryStringMap<std::string, Id, 0> map;
	map.add("ababababaab", 1);
	map.add("abababababababaccc", 2);
	map.add("abababababababaer", 3);
	map.add("abababababababber", 4);
	map.add("abababababababaaaa", 5);

	// map.print(std::cout);
	// std::cout << std::endl << map.getByteSize() << " : " << map.getUncompressedByteSize() << std::endl;

	REQUIRE(map.find("ababababaab") == 1);
	REQUIRE(map.find("abababababababaccc") == 2);
	REQUIRE(map.find("abababababababaer") == 3);
	REQUIRE(map.find("abababababababber") == 4);
	REQUIRE(map.find("abababababababaaaa") == 5);
	REQUIRE(map.find("abababab") == 0);
}

TEST_CASE("wstring")
{
	LowMemoryStringMap<std::wstring, Id, 0> map;

	FilePath filePath(L"data/LowMemoryStringMapTestSuite/names.txt");
	std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(filePath);

	std::vector<std::wstring> names;
	for (std::string line: textAccess->getAllLines())
	{
		names.emplace_back(utility::decodeFromUtf8(line.substr(0, line.find("\n"))));
	}

	for (size_t i = 0; i < names.size(); i++)
	{
		map.add(names[i], i);
	}

	// map.print(std::wcout);
	// std::cout << std::endl << map.getByteSize() << " : " << map.getUncompressedByteSize() << std::endl;

	for (size_t i = 0; i < names.size(); i++)
	{
		REQUIRE(map.find(names[i]) == i);
	}

	// TS_ASSERT(map.getByteSize() < map.getUncompressedByteSize());
}
