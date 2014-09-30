#include "cxxtest/TestSuite.h"

#include "utility/text/Dictionary.h"

class DictionaryTestSuite: public CxxTest::TestSuite
{
public:
	void test_does_not_find_unsaved_word()
	{
		Dictionary dictionary;
		TS_ASSERT_EQUALS("", dictionary.getWord(12));
	}

	void test_can_save_word_and_retrieve_it_with_id()
	{
		Dictionary dictionary;
		Id id = dictionary.getWordId("hello world!");

		TS_ASSERT_LESS_THAN(0, id);
		TS_ASSERT_EQUALS("hello world!", dictionary.getWord(id));
	}

	void test_can_save_multiple_words_and_retrieve_it_with_id()
	{
		Dictionary dictionary;
		std::deque<Id> ids = dictionary.getWordIds("hello world!", " ");

		TS_ASSERT_EQUALS(2, ids.size());
		TS_ASSERT_EQUALS("hello", dictionary.getWord(ids.front()));
		TS_ASSERT_EQUALS("world!", dictionary.getWord(ids.back()));
		TS_ASSERT_EQUALS("hello world!", dictionary.getWord(ids, " "));
	}

	void test_next_word_gets_different_id()
	{
		Dictionary dictionary;
		Id id = dictionary.getWordId("hello world!");
		Id id2 = dictionary.getWordId("foobar");

		TS_ASSERT_LESS_THAN(0, id);
		TS_ASSERT_LESS_THAN(0, id2);
		TS_ASSERT_LESS_THAN(id, id2);

		TS_ASSERT_EQUALS("hello world!", dictionary.getWord(id));
		TS_ASSERT_EQUALS("foobar", dictionary.getWord(id2));
	}
};
