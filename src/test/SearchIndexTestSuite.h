#include "cxxtest/TestSuite.h"

#include "data/SearchIndex.h"

class SearchIndexTestSuite : public CxxTest::TestSuite
{
public:

	// void clear();

	// SearchNode* addNode(const std::string& fullName);
	// SearchNode* getNode(const std::string& fullName) const;

	// std::vector<std::string> findFuzzyMatches(const std::string& query) const;

	void test_add_node()
	{
		SearchIndex index;
		SearchIndex::SearchNode* node = index.addNode("util");

		TS_ASSERT(node);
		TS_ASSERT_EQUALS("util", node->getName());
		TS_ASSERT_EQUALS("util", node->getFullName());

		TS_ASSERT(node->getNameId());
		TS_ASSERT(!node->getFirstTokenId());
		TS_ASSERT(!node->getParent());
	}

	void test_get_node()
	{
		SearchIndex index;
		index.addNode("util");
		SearchIndex::SearchNode* node = index.getNode("util");

		TS_ASSERT(node);
		TS_ASSERT_EQUALS("util", node->getName());
		TS_ASSERT_EQUALS("util", node->getFullName());

		TS_ASSERT(node->getNameId());
		TS_ASSERT(!node->getFirstTokenId());
		TS_ASSERT(!node->getParent());

		node = index.getNode("math");
		TS_ASSERT(!node);
	}

	void test_add_hierarchy_node()
	{
		SearchIndex index;
		SearchIndex::SearchNode* node = index.addNode("util::math::pow");

		TS_ASSERT(node);
		TS_ASSERT_EQUALS("pow", node->getName());
		TS_ASSERT_EQUALS("util::math::pow", node->getFullName());

		TS_ASSERT(node->getNameId());
		TS_ASSERT(!node->getFirstTokenId());

		TS_ASSERT(node->getParent());
		TS_ASSERT_EQUALS("math", node->getParent()->getName());

		TS_ASSERT(node->getParent()->getParent());
		TS_ASSERT_EQUALS("util", node->getParent()->getParent()->getName());
	}

	void test_reuse_hierarchy_node()
	{
		SearchIndex index;
		SearchIndex::SearchNode* node = index.addNode("math::pow");
		SearchIndex::SearchNode* node2 = index.addNode("math::floor");

		TS_ASSERT(node);
		TS_ASSERT(node2);

		TS_ASSERT_EQUALS("pow", node->getName());
		TS_ASSERT_EQUALS("floor", node2->getName());

		TS_ASSERT_EQUALS(node->getParent(), node2->getParent());
	}

	void test_clear()
	{
		SearchIndex index;
		index.addNode("math");
		index.addNode("string");

		TS_ASSERT(index.getNode("math"));

		index.clear();

		TS_ASSERT(!index.getNode("math"));
	}

	void test_fuzzy_matching()
	{
		SearchIndex index;
		index.addNode("util");
		index.addNode("math");
		index.addNode("string");

		std::vector<SearchIndex::SearchMatch> matches = index.findFuzzyMatches("u");

		TS_ASSERT_EQUALS(1, matches.size());
		TS_ASSERT_EQUALS("util", matches[0].fullName);

		TS_ASSERT_EQUALS(1, matches[0].indices.size());
		TS_ASSERT_EQUALS(0, matches[0].indices[0]);

		matches = index.findFuzzyMatches("");
		TS_ASSERT_EQUALS(0, matches.size());
	}

	void test_fuzzy_matching_is_case_insensitive()
	{
		SearchIndex index;
		index.addNode("util");
		index.addNode("MATH");

		std::vector<SearchIndex::SearchMatch> matches = index.findFuzzyMatches("t");

		TS_ASSERT_EQUALS(2, matches.size());
		TS_ASSERT_EQUALS("MATH", matches[0].fullName);
		TS_ASSERT_EQUALS("util", matches[1].fullName);

		matches = index.findFuzzyMatches("T");

		TS_ASSERT_EQUALS(2, matches.size());
		TS_ASSERT_EQUALS("MATH", matches[0].fullName);
		TS_ASSERT_EQUALS("util", matches[1].fullName);
	}

	void test_fuzzy_matching_wheighs_by_distance_and_alphabet()
	{
		SearchIndex index;
		index.addNode("util");
		index.addNode("math");
		index.addNode("string");

		std::vector<SearchIndex::SearchMatch> matches = index.findFuzzyMatches("t");

		TS_ASSERT_EQUALS(3, matches.size());
		TS_ASSERT_EQUALS("string", matches[0].fullName);
		TS_ASSERT_EQUALS("util", matches[1].fullName);
		TS_ASSERT_EQUALS("math", matches[2].fullName);

		TS_ASSERT_EQUALS(1, matches[0].indices.size());
		TS_ASSERT_EQUALS(1, matches[0].indices[0]);

		TS_ASSERT_EQUALS(1, matches[1].indices.size());
		TS_ASSERT_EQUALS(1, matches[1].indices[0]);

		TS_ASSERT_EQUALS(1, matches[2].indices.size());
		TS_ASSERT_EQUALS(2, matches[2].indices[0]);
	}

	void test_fuzzy_matching_wheighs_higher_by_uppercase()
	{
		SearchIndex index;
		index.addNode("uTil");
		index.addNode("string");

		std::vector<SearchIndex::SearchMatch> matches = index.findFuzzyMatches("t");

		TS_ASSERT_EQUALS(2, matches.size());
		TS_ASSERT_EQUALS("uTil", matches[0].fullName);
		TS_ASSERT_EQUALS("string", matches[1].fullName);
	}

	void test_fuzzy_matching_wheighs_higher_on_consecutive_letters()
	{
		SearchIndex index;
		index.addNode("oaabbcc");
		index.addNode("ocbaabc");

		std::vector<SearchIndex::SearchMatch> matches = index.findFuzzyMatches("abc");

		TS_ASSERT_EQUALS(2, matches.size());
		TS_ASSERT_EQUALS("ocbaabc", matches[0].fullName);
		TS_ASSERT_EQUALS("oaabbcc", matches[1].fullName);
	}

	void test_fuzzy_matching_in_hierarchy()
	{
		SearchIndex index;
		index.addNode("util::math::ceil");
		index.addNode("util::math::floor");
		index.addNode("util::string::concat");

		std::vector<SearchIndex::SearchMatch> matches = index.findFuzzyMatches("t");

		TS_ASSERT_EQUALS(1, matches.size());
		TS_ASSERT_EQUALS("util", matches[0].fullName);

		matches = index.findFuzzyMatches("uml");

		TS_ASSERT_EQUALS(2, matches.size());
		TS_ASSERT_EQUALS("util::math::floor", matches[0].fullName);
		TS_ASSERT_EQUALS("util::math::ceil", matches[1].fullName);
	}

	void test_fuzzy_matching_in_hierarchy_respects_collin()
	{
		SearchIndex index;
		index.addNode("util::math::ceil");
		index.addNode("util::math::floor");
		index.addNode("util::string::concat");

		std::vector<SearchIndex::SearchMatch> matches = index.findFuzzyMatches("u:i");

		TS_ASSERT_EQUALS(2, matches.size());
		TS_ASSERT_EQUALS("util::string", matches[0].fullName);
		TS_ASSERT_EQUALS("util::math::ceil", matches[1].fullName);

		matches = index.findFuzzyMatches("u:t:i");

		TS_ASSERT_EQUALS(1, matches.size());
		TS_ASSERT_EQUALS("util::math::ceil", matches[0].fullName);
	}

	void test_fuzzy_matching_in_hierarchy_weighs_front_letters_higher()
	{
		SearchIndex index;
		index.addNode("abc::dfe::ghi");
		index.addNode("abc::hgi");

		std::vector<SearchIndex::SearchMatch> matches = index.findFuzzyMatches("g");

		TS_ASSERT_EQUALS(2, matches.size());
		TS_ASSERT_EQUALS("abc::dfe::ghi", matches[0].fullName);
		TS_ASSERT_EQUALS("abc::hgi", matches[1].fullName);
	}

	void test_fuzzy_matching_with_defined_start_node()
	{
		SearchIndex index;
		index.addNode("math::ceil");
		index.addNode("math::floor");
		index.addNode("string::concat");

		std::vector<SearchIndex::SearchMatch> matches = index.findFuzzyMatches("\"math\"c");

		TS_ASSERT_EQUALS(1, matches.size());
		TS_ASSERT_EQUALS("math::ceil", matches[0].fullName);

		matches = index.findFuzzyMatches("\"mathc");
		TS_ASSERT_EQUALS(0, matches.size());

		matches = index.findFuzzyMatches("math\"c");
		TS_ASSERT_EQUALS(0, matches.size());

		matches = index.findFuzzyMatches("\"mat\"h\"c");
		TS_ASSERT_EQUALS(0, matches.size());
	}
};
