#include "cxxtest/TestSuite.h"

#include "utility/utilityString.h"

#include "data/search/SearchIndex.h"
#include "data/search/SearchMatch.h"

class SearchIndexTestSuite : public CxxTest::TestSuite
{
public:
	void test_add_node()
	{
		SearchIndex index;
		SearchNode* node = index.addNode(createNameHierarchy("util"));

		TS_ASSERT(node);
		TS_ASSERT_EQUALS("util", node->getName());
		TS_ASSERT_EQUALS("util", node->getFullName());

		TS_ASSERT(node->getNameId());
		TS_ASSERT(!node->getFirstTokenId());
		TS_ASSERT(node->getParent());
		TS_ASSERT(!node->getParent()->getNameId());
	}

	void test_get_node()
	{
		SearchIndex index;
		index.addNode(createNameHierarchy("util"));
		SearchNode* node = index.getNode("util");

		TS_ASSERT(node);
		TS_ASSERT_EQUALS("util", node->getName());
		TS_ASSERT_EQUALS("util", node->getFullName());

		TS_ASSERT(node->getNameId());
		TS_ASSERT(!node->getFirstTokenId());
		TS_ASSERT(node->getParent());
		TS_ASSERT(!node->getParent()->getNameId());

		node = index.getNode("math");
		TS_ASSERT(!node);
	}

	void test_add_hierarchy_node()
	{
		SearchIndex index;
		SearchNode* node = index.addNode(createNameHierarchy("util::math::pow"));

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
		SearchNode* node1 = index.addNode(createNameHierarchy("math::pow"));
		SearchNode* node2 = index.addNode(createNameHierarchy("math::floor"));

		TS_ASSERT(node1);
		TS_ASSERT(node2);

		TS_ASSERT_EQUALS("pow", node1->getName());
		TS_ASSERT_EQUALS("floor", node2->getName());

		TS_ASSERT_EQUALS(node1->getParent(), node2->getParent());
	}

	void test_remove_nodes()
	{
		SearchIndex index;
		index.addNode(createNameHierarchy("util::math::pow"));
		index.addNode(createNameHierarchy("util::math::floor"));

		index.removeNode(index.getNode("util::math::pow"));

		TS_ASSERT(!index.getNode("util::math::pow"));
		TS_ASSERT(index.getNode("util::math::floor"));
		TS_ASSERT(index.getNode("util::math"));

		index.removeNode(index.getNode("util::math"));

		TS_ASSERT(!index.getNode("util::math::floor"));
		TS_ASSERT(!index.getNode("util::math"));
		TS_ASSERT(index.getNode("util"));
	}

	void test_remove_unreferenced_nodes()
	{
		SearchIndex index;
		SearchNode* node1 = index.addNode(createNameHierarchy("util::math::pow"));
		SearchNode* node2 = index.addNode(createNameHierarchy("util::math::floor"));

		node1->addTokenId(1);
		node2->addTokenId(2);

		TS_ASSERT(index.getNode("util")->hasTokenIdsRecursive());

		TS_ASSERT(!index.removeNodeIfUnreferencedRecursive(index.getNode("util")));
		TS_ASSERT(!index.removeNodeIfUnreferencedRecursive(index.getNode("util::math")));
		TS_ASSERT(!index.removeNodeIfUnreferencedRecursive(index.getNode("util::math::pow")));

		node1->removeTokenId(1);

		TS_ASSERT(index.removeNodeIfUnreferencedRecursive(index.getNode("util::math::pow")));
		TS_ASSERT(!index.getNode("util::math::pow"));
		TS_ASSERT(index.getNode("util::math"));
		TS_ASSERT(index.getNode("util"));

		node2->removeTokenId(2);

		TS_ASSERT(index.removeNodeIfUnreferencedRecursive(index.getNode("util::math")));

		TS_ASSERT(!index.getNode("util::math"));
		TS_ASSERT(!index.getNode("util"));
	}

	void test_clear()
	{
		SearchIndex index;
		index.addNode(createNameHierarchy("math"));
		index.addNode(createNameHierarchy("string"));

		TS_ASSERT(index.getNode("math"));

		index.clear();

		TS_ASSERT(!index.getNode("math"));
		TS_ASSERT(!index.getNode("string"));
	}

	void test_fuzzy_matching()
	{
		SearchIndex index;
		index.addNode(createNameHierarchy("util"));
		index.addNode(createNameHierarchy("math"));
		index.addNode(createNameHierarchy("string"));

		std::vector<SearchMatch> matches = index.runFuzzySearchAndGetMatches("u");

		TS_ASSERT_EQUALS(1, matches.size());
		TS_ASSERT_EQUALS("util", matches[0].fullName);

		TS_ASSERT_EQUALS(1, matches[0].indices.size());
		TS_ASSERT_EQUALS(0, matches[0].indices[0]);

		matches = index.runFuzzySearchAndGetMatches("");
		TS_ASSERT_EQUALS(3, matches.size());
		TS_ASSERT_EQUALS("math", matches[0].fullName);
		TS_ASSERT_EQUALS("string", matches[1].fullName);
		TS_ASSERT_EQUALS("util", matches[2].fullName);

		TS_ASSERT_EQUALS(0, matches[0].weight);
		TS_ASSERT_EQUALS(0, matches[1].weight);
		TS_ASSERT_EQUALS(0, matches[2].weight);
	}

	void test_fuzzy_matching_is_case_insensitive()
	{
		SearchIndex index;
		index.addNode(createNameHierarchy("util"));
		index.addNode(createNameHierarchy("MATH"));

		std::vector<SearchMatch> matches = index.runFuzzySearchAndGetMatches("t");

		TS_ASSERT_EQUALS(2, matches.size());
		TS_ASSERT_EQUALS("util", matches[0].fullName);
		TS_ASSERT_EQUALS("MATH", matches[1].fullName);

		matches = index.runFuzzySearchAndGetMatches("T");

		TS_ASSERT_EQUALS(2, matches.size());
		TS_ASSERT_EQUALS("util", matches[0].fullName);
		TS_ASSERT_EQUALS("MATH", matches[1].fullName);
	}

	void test_fuzzy_matching_wheighs_by_distance_and_alphabet()
	{
		SearchIndex index;
		index.addNode(createNameHierarchy("util"));
		index.addNode(createNameHierarchy("math"));
		index.addNode(createNameHierarchy("string"));

		std::vector<SearchMatch> matches = index.runFuzzySearchAndGetMatches("t");

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
		index.addNode(createNameHierarchy("uTil"));
		index.addNode(createNameHierarchy("string"));

		std::vector<SearchMatch> matches = index.runFuzzySearchAndGetMatches("t");

		TS_ASSERT_EQUALS(2, matches.size());
		TS_ASSERT_EQUALS("uTil", matches[0].fullName);
		TS_ASSERT_EQUALS("string", matches[1].fullName);
	}

	void test_fuzzy_matching_wheighs_higher_on_consecutive_letters()
	{
		SearchIndex index;
		index.addNode(createNameHierarchy("oaabbcc"));
		index.addNode(createNameHierarchy("ocbcabc"));

		std::vector<SearchMatch> matches = index.runFuzzySearchAndGetMatches("abc");

		TS_ASSERT_EQUALS(2, matches.size());
		TS_ASSERT_EQUALS("ocbcabc", matches[0].fullName);
		TS_ASSERT_EQUALS("oaabbcc", matches[1].fullName);
	}

	void test_fuzzy_matching_in_hierarchy()
	{
		SearchIndex index;
		index.addNode(createNameHierarchy("util::math::ceil"));
		index.addNode(createNameHierarchy("util::math::floor"));
		index.addNode(createNameHierarchy("util::string::concat"));

		std::vector<SearchMatch> matches = index.runFuzzySearchAndGetMatches("t");

		TS_ASSERT_EQUALS(6, matches.size());
		TS_ASSERT_EQUALS("util", matches[0].fullName);
		TS_ASSERT_EQUALS("util::math", matches[1].fullName);
		TS_ASSERT_EQUALS("util::math::ceil", matches[2].fullName);
		TS_ASSERT_EQUALS("util::math::floor", matches[3].fullName);
		TS_ASSERT_EQUALS("util::string", matches[4].fullName);
		TS_ASSERT_EQUALS("util::string::concat", matches[5].fullName);

		matches = index.runFuzzySearchAndGetMatches("uml");

		TS_ASSERT_EQUALS(2, matches.size());
		TS_ASSERT_EQUALS("util::math::floor", matches[0].fullName);
		TS_ASSERT_EQUALS("util::math::ceil", matches[1].fullName);
	}

	void test_fuzzy_matching_in_hierarchy_respects_collin()
	{
		SearchIndex index;
		index.addNode(createNameHierarchy("util::math::ceil"));
		index.addNode(createNameHierarchy("util::math::floor"));
		index.addNode(createNameHierarchy("util::string::concat"));

		std::vector<SearchMatch> matches = index.runFuzzySearchAndGetMatches("u:i");

		TS_ASSERT_EQUALS(3, matches.size());
		TS_ASSERT_EQUALS("util::string", matches[0].fullName);
		TS_ASSERT_EQUALS("util::string::concat", matches[1].fullName);
		TS_ASSERT_EQUALS("util::math::ceil", matches[2].fullName);

		matches = index.runFuzzySearchAndGetMatches("u:t:i");

		TS_ASSERT_EQUALS(1, matches.size());
		TS_ASSERT_EQUALS("util::math::ceil", matches[0].fullName);
	}

	void test_fuzzy_matching_in_hierarchy_weighs_front_letters_higher()
	{
		SearchIndex index;
		index.addNode(createNameHierarchy("abc::dfe::ghi"));
		index.addNode(createNameHierarchy("abc::hgi"));

		std::vector<SearchMatch> matches = index.runFuzzySearchAndGetMatches("g");

		TS_ASSERT_EQUALS(2, matches.size());
		TS_ASSERT_EQUALS("abc::dfe::ghi", matches[0].fullName);
		TS_ASSERT_EQUALS("abc::hgi", matches[1].fullName);
	}

private:
	NameHierarchy createNameHierarchy(std::string s) const
	{
		NameHierarchy nameHierarchy;
		for (std::string element: utility::splitToVector(s, "::"))
		{
			nameHierarchy.push(std::make_shared<NameElement>(element));
		}
		return nameHierarchy;
	}
};
