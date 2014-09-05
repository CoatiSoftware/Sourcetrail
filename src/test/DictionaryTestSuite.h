#include "cxxtest/TestSuite.h"

#include "utility/text/Dictionary.h"

class DictionaryTestSuite: public CxxTest::TestSuite
{
public:
	void test_get_instance()
	{
		TS_ASSERT(Dictionary::getInstance());
	}

	void test_does_not_find_unsaved_word()
	{
		TS_ASSERT_EQUALS("", Dictionary::getInstance()->getWord(12));
	}

	void test_can_save_word_and_retrieve_it_with_id()
	{
		Id id = Dictionary::getInstance()->getWordId("hello world!");

		TS_ASSERT_LESS_THAN(0, id);
		TS_ASSERT_EQUALS("hello world!", Dictionary::getInstance()->getWord(id));
	}

	void test_can_save_multiple_words_and_retrieve_it_with_id()
	{
		std::deque<Id> ids = Dictionary::getInstance()->getWordIds("hello world!", " ");

		TS_ASSERT_EQUALS(2, ids.size());
		TS_ASSERT_EQUALS("hello", Dictionary::getInstance()->getWord(ids.front()));
		TS_ASSERT_EQUALS("world!", Dictionary::getInstance()->getWord(ids.back()));
		TS_ASSERT_EQUALS("hello world!", Dictionary::getInstance()->getWord(ids, " "));
	}

	void test_next_word_gets_different_id()
	{
		Id id = Dictionary::getInstance()->getWordId("hello world!");
		Id id2 = Dictionary::getInstance()->getWordId("foobar");

		TS_ASSERT_LESS_THAN(0, id);
		TS_ASSERT_LESS_THAN(0, id2);
		TS_ASSERT_LESS_THAN(id, id2);

		TS_ASSERT_EQUALS("hello world!", Dictionary::getInstance()->getWord(id));
		TS_ASSERT_EQUALS("foobar", Dictionary::getInstance()->getWord(id2));
	}
};
