#include "cxxtest/TestSuite.h"

#include "utility/utilityString.h"

#include "data/search/SearchIndex.h"

class SearchIndexTestSuite : public CxxTest::TestSuite
{
public:

	void test_search_index_finds_id_of_element_added()
	{
		SearchIndex index;
		index.addNode(1, NameHierarchy::deserialize("foo\tsvoid\tp() const"));
		index.finishSetup();
		std::vector<SearchResult> results = index.search("oo", 0);

		TS_ASSERT_EQUALS(1, results.size());
		TS_ASSERT_EQUALS(1, results[0].elementIds.size());
		TS_ASSERT_DIFFERS(results[0].elementIds.end(), results[0].elementIds.find(1));
	}

	void test_search_index_finds_correct_indices_for_query()
	{
		SearchIndex index;
		index.addNode(1, NameHierarchy::deserialize("foo\tsvoid\tp() const"));
		index.finishSetup();
		std::vector<SearchResult> results = index.search("oo", 0);

		TS_ASSERT_EQUALS(1, results.size());
		TS_ASSERT_EQUALS(2, results[0].indices.size());
		TS_ASSERT_EQUALS(1, results[0].indices[0]);
		TS_ASSERT_EQUALS(2, results[0].indices[1]);
	}

	void test_search_index_finds_ids_for_ambiguous_query()
	{
		SearchIndex index;
		index.addNode(1, NameHierarchy::deserialize("for\tsvoid\tp() const"));
		index.addNode(2, NameHierarchy::deserialize("fos\tsvoid\tp() const"));
		index.finishSetup();
		std::vector<SearchResult> results = index.search("fo", 0);

		TS_ASSERT_EQUALS(2, results.size());
		TS_ASSERT_EQUALS(1, results[0].elementIds.size());
		TS_ASSERT_DIFFERS(results[0].elementIds.end(), results[0].elementIds.find(1));
		TS_ASSERT_EQUALS(1, results[1].elementIds.size());
		TS_ASSERT_DIFFERS(results[1].elementIds.end(), results[1].elementIds.find(2));
	}

	void test_search_index_does_not_find_anything_after_clear()
	{
		SearchIndex index;
		index.addNode(1, NameHierarchy::deserialize("foo\tsvoid\tp() const"));
		index.finishSetup();
		index.clear();
		std::vector<SearchResult> results = index.search("oo", 0);

		TS_ASSERT_EQUALS(0, results.size());
	}

	void test_search_index_does_not_find_all_results_when_max_amount_is_limited()
	{
		SearchIndex index;
		index.addNode(1, NameHierarchy::deserialize("foo1\tsvoid\tp() const"));
		index.addNode(2, NameHierarchy::deserialize("foo2\tsvoid\tp() const"));
		index.finishSetup();
		std::vector<SearchResult> results = index.search("oo", 1);

		TS_ASSERT_EQUALS(1, results.size());
	}

	void test_search_index_query_is_case_insensitive()
	{
		SearchIndex index;
		index.addNode(1, NameHierarchy::deserialize("foo1\tsvoid\tp() const"));
		index.addNode(2, NameHierarchy::deserialize("FOO2\tsvoid\tp() const"));
		index.finishSetup();
		std::vector<SearchResult> results = index.search("oo", 0);

		TS_ASSERT_EQUALS(2, results.size());
	}

	void test_search_index_rates_higher_on_consecutive_letters()
	{

		SearchIndex index;
		index.addNode(1, NameHierarchy::deserialize("oaabbcc\tsvoid\tp() const"));
		index.addNode(2, NameHierarchy::deserialize("ocbcabc\tsvoid\tp() const"));
		index.finishSetup();
		std::vector<SearchResult> results = index.search("abc", 0);

		TS_ASSERT_EQUALS(2, results.size());
		TS_ASSERT_EQUALS("ocbcabc", results[0].text);
		TS_ASSERT_EQUALS("oaabbcc", results[1].text);
	}
};
