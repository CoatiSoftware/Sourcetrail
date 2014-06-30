#include "cxxtest/TestSuite.h"

#include "utility/utilityString.h"

class UtilityStringTestSuite : public CxxTest::TestSuite
{
public:
	void test_split_with_char_delimiter()
	{
		std::vector<std::string> result = utility::split("A,B,C", ',');

		TS_ASSERT_EQUALS(result.size(), 3);
		TS_ASSERT_EQUALS(result[0], "A");
		TS_ASSERT_EQUALS(result[1], "B");
		TS_ASSERT_EQUALS(result[2], "C");
	}

	void test_split_with_string_delimiter()
	{
		std::vector<std::string> result = utility::split("A->B>C", "->");

		TS_ASSERT_EQUALS(result.size(), 2);
		TS_ASSERT_EQUALS(result[0], "A");
		TS_ASSERT_EQUALS(result[1], "B>C");
	}

	void test_split_on_empty_string()
	{
		std::vector<std::string> result = utility::split("", "->");

		TS_ASSERT_EQUALS(result.size(), 1);
		TS_ASSERT_EQUALS(result[0], "");
	}

	void test_split_with_unused_delimiter()
	{
		std::vector<std::string> result = utility::split("A:B:C", ";");

		TS_ASSERT_EQUALS(result.size(), 1);
		TS_ASSERT_EQUALS(result[0], "A:B:C");
	}

	void test_split_with_delimiters_next_to_each()
	{
		std::vector<std::string> result = utility::split("A::B:C", ':');

		TS_ASSERT_EQUALS(result.size(), 4);
		TS_ASSERT_EQUALS(result[0], "A");
		TS_ASSERT_EQUALS(result[1], "");
		TS_ASSERT_EQUALS(result[2], "B");
		TS_ASSERT_EQUALS(result[3], "C");
	}
};
