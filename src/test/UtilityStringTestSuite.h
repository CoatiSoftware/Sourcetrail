#include "cxxtest/TestSuite.h"

#include "utility/utilityString.h"

class UtilityStringTestSuite : public CxxTest::TestSuite
{
public:
	void test_split_with_char_delimiter()
	{
		std::vector<std::string> result = utility::split<std::vector<std::string> >("A,B,C", ',');

		TS_ASSERT_EQUALS(result.size(), 3);
		TS_ASSERT_EQUALS(result[0], "A");
		TS_ASSERT_EQUALS(result[1], "B");
		TS_ASSERT_EQUALS(result[2], "C");
	}

	void test_split_with_string_delimiter()
	{
		std::vector<std::string> result = utility::split<std::vector<std::string> >("A->B>C", "->");

		TS_ASSERT_EQUALS(result.size(), 2);
		TS_ASSERT_EQUALS(result[0], "A");
		TS_ASSERT_EQUALS(result[1], "B>C");
	}

	void test_split_on_empty_string()
	{
		std::vector<std::string> result = utility::split<std::vector<std::string> >("", "->");

		TS_ASSERT_EQUALS(result.size(), 1);
		TS_ASSERT_EQUALS(result[0], "");
	}

	void test_split_with_unused_delimiter()
	{
		std::vector<std::string> result = utility::split<std::vector<std::string> >("A:B:C", ";");

		TS_ASSERT_EQUALS(result.size(), 1);
		TS_ASSERT_EQUALS(result[0], "A:B:C");
	}

	void test_split_with_delimiters_next_to_each()
	{
		std::vector<std::string> result = utility::split<std::vector<std::string> >("A::B:C", ':');

		TS_ASSERT_EQUALS(result.size(), 4);
		TS_ASSERT_EQUALS(result[0], "A");
		TS_ASSERT_EQUALS(result[1], "");
		TS_ASSERT_EQUALS(result[2], "B");
		TS_ASSERT_EQUALS(result[3], "C");
	}

	void test_split_with_delimiter_at_start()
	{
		std::vector<std::string> result = utility::split<std::vector<std::string> >(":B:C", ':');

		TS_ASSERT_EQUALS(result.size(), 3);
		TS_ASSERT_EQUALS(result[0], "");
		TS_ASSERT_EQUALS(result[1], "B");
		TS_ASSERT_EQUALS(result[2], "C");
	}

	void test_split_with_delimiter_at_end()
	{
		std::vector<std::string> result = utility::split<std::vector<std::string> >("B:C:", ':');

		TS_ASSERT_EQUALS(result.size(), 3);
		TS_ASSERT_EQUALS(result[0], "B");
		TS_ASSERT_EQUALS(result[1], "C");
		TS_ASSERT_EQUALS(result[2], "");
	}

	void test_tokenize_with_string()
	{
		std::vector<std::string> result = utility::tokenize<std::vector<std::string> >("A->B->C", "->");

		TS_ASSERT_EQUALS(result.size(), 5);
		TS_ASSERT_EQUALS(result[0], "A");
		TS_ASSERT_EQUALS(result[1], "->");
		TS_ASSERT_EQUALS(result[2], "B");
		TS_ASSERT_EQUALS(result[3], "->");
		TS_ASSERT_EQUALS(result[4], "C");
	}

	void test_tokenize_with_string_and_delimiter_at_start()
	{
		std::vector<std::string> result = utility::tokenize<std::vector<std::string> >("->B", "->");

		TS_ASSERT_EQUALS(result.size(), 2);
		TS_ASSERT_EQUALS(result[0], "->");
		TS_ASSERT_EQUALS(result[1], "B");
	}

	void test_tokenize_with_string_and_delimiter_at_end()
	{
		std::vector<std::string> result = utility::tokenize<std::vector<std::string> >("C+", '+');

		TS_ASSERT_EQUALS(result.size(), 2);
		TS_ASSERT_EQUALS(result[0], "C");
		TS_ASSERT_EQUALS(result[1], "+");
	}

	void test_tokenize_with_vector()
	{
		std::vector<std::string> result = utility::tokenize<std::vector<std::string> >("A->B=C->D", "->");
		result = utility::tokenize<std::vector<std::string>>(result, "=");

		TS_ASSERT_EQUALS(result.size(), 7);
		TS_ASSERT_EQUALS(result[0], "A");
		TS_ASSERT_EQUALS(result[1], "->");
		TS_ASSERT_EQUALS(result[2], "B");
		TS_ASSERT_EQUALS(result[3], "=");
		TS_ASSERT_EQUALS(result[4], "C");
		TS_ASSERT_EQUALS(result[5], "->");
		TS_ASSERT_EQUALS(result[6], "D");
	}

	void test_substr_after_with_single_delimiter_occurence()
	{
		TS_ASSERT_EQUALS(utility::substrAfter("foo bar", ' '), "bar");
	}

	void test_substr_after_with_multiple_delimiter_occurences()
	{
		TS_ASSERT_EQUALS(utility::substrAfter("foo bar foo", ' '), "bar foo");
	}

	void test_substr_after_with_no_delimiter_occurence()
	{
		TS_ASSERT_EQUALS(utility::substrAfter("foobar", ' '), "foobar");
	}

	void test_substr_after_with_delimiter_at_start()
	{
		TS_ASSERT_EQUALS(utility::substrAfter(" foobar", ' '), "foobar");
	}

	void test_substr_after_with_delimiter_at_end()
	{
		TS_ASSERT_EQUALS(utility::substrAfter("foobar ", ' '), "");
	}

	void test_empty_string_is_detected_as_prefix_of_any_other_string()
	{
		const std::string foo = "foo";

		TS_ASSERT(utility::isPrefix("", foo));
	}

	void test_prefix_of_bigger_text_is_detected_as_prefix()
	{
		const std::string foobar = "foobar";
		const std::string foo = "foo";

		TS_ASSERT(utility::isPrefix(foo, foobar));
	}

	void test_prefix_is_detected_as_prefix_of_self()
	{
		const std::string foo = "foo";

		TS_ASSERT(utility::isPrefix(foo, foo));
	}

	void test_different_texts_are_not_detected_of_prefixes_of_each_other()
	{
		const std::string foo = "foo";
		const std::string bar = "bar";

		TS_ASSERT(!utility::isPrefix(foo, bar));
		TS_ASSERT(!utility::isPrefix(bar, foo));
	}
};
