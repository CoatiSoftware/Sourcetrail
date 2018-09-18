#include <cxxtest/TestSuite.h>

#include "LowMemoryStringMap.h"
#include "TextAccess.h"
#include "types.h"
#include "utilityString.h"

class LowMemoryStringMapTestSuite : public CxxTest::TestSuite
{
public:
	void test_roughly_everything()
	{
		LowMemoryStringMap<std::string, Id, 0> map;

		map.add("abcdefg", 2);
		map.add("abcdefgerlitz", 1);
		map.add("abcdefghij", 3);
		map.add("abc", 4);

		// map.print(std::cout);
		// std::cout << std::endl << map.getByteSize() << " : " << map.getUncompressedByteSize() << std::endl;

		TS_ASSERT(map.find("abcdefgerlitz") == 1);
		TS_ASSERT(map.find("abcdefg") == 2);
		TS_ASSERT(map.find("abcdefghij") == 3);
		TS_ASSERT(map.find("abc") == 4);
		TS_ASSERT(map.find("bc") == 0);
		TS_ASSERT(map.find("") == 0);
		TS_ASSERT(map.find(";asdfl;kjasd;flkasdf") == 0);

		// TS_ASSERT(map.getByteSize() < map.getUncompressedByteSize());
	}

	void test_cannot_find_element_after_creation()
	{
		LowMemoryStringMap<std::string, Id, 0> map;

		TS_ASSERT(map.find("a") == 0);
	}

	void test_find_element()
	{
		LowMemoryStringMap<std::string, Id, 0> map;
		map.add("a", 1);

		TS_ASSERT(map.find("a") == 1);
		TS_ASSERT(map.find("b") == 0);
	}

	void test_find_fully_different_elements()
	{
		LowMemoryStringMap<std::string, Id, 0> map;
		map.add("a", 1);
		map.add("b", 2);

		TS_ASSERT(map.find("a") == 1);
		TS_ASSERT(map.find("b") == 2);
	}

	void test_find_similar_short_elements()
	{
		LowMemoryStringMap<std::string, Id, 0> map;
		map.add("ab", 1);
		map.add("ac", 2);

		TS_ASSERT(map.find("ab") == 1);
		TS_ASSERT(map.find("ac") == 2);
		TS_ASSERT(map.find("bc") == 0);
	}

	void test_find_similar_long_elements()
	{
		LowMemoryStringMap<std::string, Id, 0> map;
		map.add("aaaaabbbbb", 1);
		map.add("aaaaaccccc", 2);
		map.add("aaaaccccc", 3);
		map.add("aaaccccc", 4);

		TS_ASSERT(map.find("aaaaabbbbb") == 1);
		TS_ASSERT(map.find("aaaaaccccc") == 2);
		TS_ASSERT(map.find("aaaaacccccc") == 0);
		TS_ASSERT(map.find("aacc") == 0);
	}

	void test_add_twice()
	{
		LowMemoryStringMap<std::string, Id, 0> map;
		map.add("abba", 1);
		map.add("abba", 2);

		TS_ASSERT(map.find("abba") == 1);
	}

	void test_find_parent_child_elements()
	{
		LowMemoryStringMap<std::string, Id, 0> map;
		map.add("a", 1);
		map.add("ab", 2);

		TS_ASSERT(map.find("a") == 1);
		TS_ASSERT(map.find("ab") == 2);
		TS_ASSERT(map.find("b") == 0);
	}

	void test_find_long_parent_child_elements()
	{
		LowMemoryStringMap<std::string, Id, 0> map;
		map.add("ababaaa", 1);
		map.add("aba", 2);

		TS_ASSERT(map.find("ababaaa") == 1);
		TS_ASSERT(map.find("aba") == 2);
		TS_ASSERT(map.find("ab") == 0);
		TS_ASSERT(map.find("") == 0);
	}

	void test_find_long_similar_prefix_elements()
	{
		LowMemoryStringMap<std::string, Id, 0> map;
		map.add("ababababaab", 1);
		map.add("abababababababaccc", 2);
		map.add("abababababababaer", 3);
		map.add("abababababababber", 4);
		map.add("abababababababaaaa", 5);

		// map.print(std::cout);
		// std::cout << std::endl << map.getByteSize() << " : " << map.getUncompressedByteSize() << std::endl;

		TS_ASSERT(map.find("ababababaab") == 1);
		TS_ASSERT(map.find("abababababababaccc") == 2);
		TS_ASSERT(map.find("abababababababaer") == 3);
		TS_ASSERT(map.find("abababababababber") == 4);
		TS_ASSERT(map.find("abababababababaaaa") == 5);
		TS_ASSERT(map.find("abababab") == 0);
	}

	void test_wstring()
	{
		LowMemoryStringMap<std::wstring, Id, 0> map;

		FilePath filePath(L"data/LowMemoryStringMapTestSuite/names.txt");
		std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(filePath);

		std::vector<std::wstring> names;
		for (std::string line : textAccess->getAllLines())
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
			TS_ASSERT(map.find(names[i]) == i);
		}

		// TS_ASSERT(map.getByteSize() < map.getUncompressedByteSize());
	}
};
