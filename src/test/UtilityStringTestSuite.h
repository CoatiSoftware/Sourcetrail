#include "cxxtest/TestSuite.h"

#include "utility/utilityString.h"

class UtilityStringTestSuite : public CxxTest::TestSuite
{
public:
	void test_split_with_char_delimiter()
	{
		std::deque<std::string> result = utility::split("A,B,C", ',');

		TS_ASSERT_EQUALS(result.size(), 3);
		TS_ASSERT_EQUALS(result.at(0), "A");
		TS_ASSERT_EQUALS(result.at(1), "B");
		TS_ASSERT_EQUALS(result.at(2), "C");
	}

	void test_split_with_string_delimiter()
	{
		std::deque<std::string> result = utility::split("A->B>C", "->");

		TS_ASSERT_EQUALS(result.size(), 2);
		TS_ASSERT_EQUALS(result.at(0), "A");
		TS_ASSERT_EQUALS(result.at(1), "B>C");
	}

	void test_split_on_empty_string()
	{
		std::deque<std::string> result = utility::split("", "->");

		TS_ASSERT_EQUALS(result.size(), 1);
		TS_ASSERT_EQUALS(result.at(0), "");
	}

	void test_split_with_unused_delimiter()
	{
		std::deque<std::string> result = utility::split("A:B:C", ";");

		TS_ASSERT_EQUALS(result.size(), 1);
		TS_ASSERT_EQUALS(result.at(0), "A:B:C");
	}

	void test_split_with_delimiters_next_to_each()
	{
		std::deque<std::string> result = utility::split("A::B:C", ':');

		TS_ASSERT_EQUALS(result.size(), 4);
		TS_ASSERT_EQUALS(result.at(0), "A");
		TS_ASSERT_EQUALS(result.at(1), "");
		TS_ASSERT_EQUALS(result.at(2), "B");
		TS_ASSERT_EQUALS(result.at(3), "C");
	}

	void test_split_with_delimiter_at_start()
	{
		std::deque<std::string> result = utility::split(":B:C", ':');

		TS_ASSERT_EQUALS(result.size(), 3);
		TS_ASSERT_EQUALS(result.at(0), "");
		TS_ASSERT_EQUALS(result.at(1), "B");
		TS_ASSERT_EQUALS(result.at(2), "C");
	}

	void test_split_with_delimiter_at_end()
	{
		std::deque<std::string> result = utility::split("B:C:", ':');

		TS_ASSERT_EQUALS(result.size(), 3);
		TS_ASSERT_EQUALS(result.at(0), "B");
		TS_ASSERT_EQUALS(result.at(1), "C");
		TS_ASSERT_EQUALS(result.at(2), "");
	}

	void test_join_with_char_delimiter()
	{
		std::deque<std::string> list;
		list.push_back("A");
		list.push_back("B");
		list.push_back("C");

		std::string result = utility::join(list, ',');
		TS_ASSERT_EQUALS(result, "A,B,C");
	}

	void test_join_with_string_delimiter()
	{
		std::deque<std::string> list;
		list.push_back("A");
		list.push_back("B");
		list.push_back("C");

		std::string result = utility::join(list, "==");
		TS_ASSERT_EQUALS(result, "A==B==C");
	}

	void test_join_on_empty_list()
	{
		std::deque<std::string> list;
		std::string result = utility::join(list, ',');
		TS_ASSERT_EQUALS(result, "");
	}

	void test_join_with_empty_strings_in_list()
	{
		std::deque<std::string> list;
		list.push_back("A");
		list.push_back("");
		list.push_back("");

		std::string result = utility::join(list, ':');
		TS_ASSERT_EQUALS(result, "A::");
	}

	void test_tokenize_with_string()
	{
		std::deque<std::string> result = utility::tokenize("A->B->C", "->");

		TS_ASSERT_EQUALS(result.size(), 5);
		TS_ASSERT_EQUALS(result.at(0), "A");
		TS_ASSERT_EQUALS(result.at(1), "->");
		TS_ASSERT_EQUALS(result.at(2), "B");
		TS_ASSERT_EQUALS(result.at(3), "->");
		TS_ASSERT_EQUALS(result.at(4), "C");
	}

	void test_tokenize_with_string_and_delimiter_at_start()
	{
		std::deque<std::string> result = utility::tokenize("->B", "->");

		TS_ASSERT_EQUALS(result.size(), 2);
		TS_ASSERT_EQUALS(result.at(0), "->");
		TS_ASSERT_EQUALS(result.at(1), "B");
	}

	void test_tokenize_with_string_and_delimiter_at_end()
	{
		std::deque<std::string> result = utility::tokenize("C+", '+');

		TS_ASSERT_EQUALS(result.size(), 2);
		TS_ASSERT_EQUALS(result.at(0), "C");
		TS_ASSERT_EQUALS(result.at(1), "+");
	}

	void test_tokenize_with_deque()
	{
		std::deque<std::string> result = utility::tokenize("A->B=C->D", "->");
		result = utility::tokenize(result, "=");

		TS_ASSERT_EQUALS(result.size(), 7);
		TS_ASSERT_EQUALS(result.at(0), "A");
		TS_ASSERT_EQUALS(result.at(1), "->");
		TS_ASSERT_EQUALS(result.at(2), "B");
		TS_ASSERT_EQUALS(result.at(3), "=");
		TS_ASSERT_EQUALS(result.at(4), "C");
		TS_ASSERT_EQUALS(result.at(5), "->");
		TS_ASSERT_EQUALS(result.at(6), "D");
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

	void test_equals_case_insensitive_with_different_cases()
	{
		const std::string foo = "FooBar";
		const std::string foo2 = "foobar";

		TS_ASSERT(utility::equalsCaseInsensitive(foo, foo2));
	}

	void test_equals_case_insensitive_with_same_cases()
	{
		const std::string foo = "foobar";
		const std::string foo2 = "foobar";

		TS_ASSERT(utility::equalsCaseInsensitive(foo, foo2));
	}

	void test_equals_case_insensitive_with_different_strings()
	{
		const std::string foo = "foo";
		const std::string foo2 = "foobar";

		TS_ASSERT(!utility::equalsCaseInsensitive(foo, foo2));
	}

	void test_replace()
	{
		TS_ASSERT_EQUALS("fubar", utility::replace("foobar", "oo", "u"));
		TS_ASSERT_EQUALS("fuuuubar", utility::replace("foobar", "o", "uu"));
		TS_ASSERT_EQUALS("bar", utility::replace("foobar", "foo", ""));
		TS_ASSERT_EQUALS("foobar", utility::replace("foobar", "", "i"));
		TS_ASSERT_EQUALS("foobar", utility::replace("foobar", "", ""));
		TS_ASSERT_EQUALS("", utility::replace("", "foo", "bar"));
		TS_ASSERT_EQUALS("foobar", utility::replace("foobar", "ba", "ba"));
	}
};
