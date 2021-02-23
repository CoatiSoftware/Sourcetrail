#include "catch.hpp"

#include "HierarchyCache.h"
#include "utility.h"

class TestEdge
{
public:
	TestEdge(Id sourceId, Id targetId, const std::vector<Id>& edgeIds)
		: m_sourceId(sourceId), m_targetId(targetId), m_edgeIds(edgeIds)
	{
		std::sort(m_edgeIds.begin(), m_edgeIds.end());
	}

	TestEdge(std::tuple<Id, Id, std::vector<size_t>> tuple)
		: m_sourceId(std::get<0>(tuple))
		, m_targetId(std::get<1>(tuple))
		, m_edgeIds(std::get<2>(tuple))
	{
		std::sort(m_edgeIds.begin(), m_edgeIds.end());
	}

	std::string toString() const
	{
		std::string ret = "s:" + std::to_string(m_sourceId) + ";";
		ret += "t:" + std::to_string(m_targetId) + ";";
		for (Id edgeId: m_edgeIds)
		{
			ret += "e:" + std::to_string(edgeId) + ";";
		}
		return ret;
	}

	const Id m_sourceId;
	const Id m_targetId;
	std::vector<Id> m_edgeIds;
};

std::vector<std::string> getSerializedInheritanceEdges(
	HierarchyCache& cache, Id nodeId, std::set<Id> nodeIds)
{
	std::vector<std::string> inheritanceEdges;
	for (const std::tuple<Id, Id, std::vector<size_t>>& edge:
		 cache.getInheritanceEdgesForNodeId(nodeId, nodeIds))
	{
		inheritanceEdges.push_back(TestEdge(edge).toString());
	}
	return inheritanceEdges;
}

TEST_CASE("HierarchyCache returns no inheritance edges for empty node set")
{
	HierarchyCache cache;
	cache.createInheritance(1, 1, 2);
	std::vector<std::string> inheritanceEdges = getSerializedInheritanceEdges(cache, 1, {});
	REQUIRE(inheritanceEdges.size() == 0);
}

TEST_CASE("HierarchyCache returns direct inheritance edge for non-empty node set")
{
	HierarchyCache cache;
	cache.createInheritance(1, 1, 2);
	std::vector<std::string> inheritanceEdges = getSerializedInheritanceEdges(cache, 1, {2});
	REQUIRE(inheritanceEdges.size() == 1);
	REQUIRE(utility::containsElement(inheritanceEdges, TestEdge(1, 2, {1}).toString()));
}

TEST_CASE("HierarchyCache returns multiple indirect inheritance edges for non-empty node set")
{
	HierarchyCache cache;
	cache.createInheritance(1, 1, 2);
	cache.createInheritance(2, 2, 3);
	std::vector<std::string> inheritanceEdges = getSerializedInheritanceEdges(cache, 1, {1, 2, 3});
	REQUIRE(inheritanceEdges.size() == 2);
	REQUIRE(utility::containsElement(inheritanceEdges, TestEdge(1, 2, {1}).toString()));
	REQUIRE(utility::containsElement(inheritanceEdges, TestEdge(1, 3, {1, 2}).toString()));
}

TEST_CASE("HierarchyCache skips inheritance for nodes not in set")
{
	HierarchyCache cache;
	cache.createInheritance(1, 1, 2);
	cache.createInheritance(2, 2, 3);
	std::vector<std::string> inheritanceEdges = getSerializedInheritanceEdges(cache, 1, {3});
	REQUIRE(inheritanceEdges.size() == 1);
	REQUIRE(utility::containsElement(inheritanceEdges, TestEdge(1, 3, {1, 2}).toString()));
}

TEST_CASE("HierarchyCache returns inheritance edges for cyclic inheritance")
{
	HierarchyCache cache;
	cache.createInheritance(1, 1, 2);
	cache.createInheritance(2, 2, 1);
	std::vector<std::string> inheritanceEdges = getSerializedInheritanceEdges(cache, 1, {1, 2});
	REQUIRE(inheritanceEdges.size() == 2);
	REQUIRE(utility::containsElement(inheritanceEdges, TestEdge(1, 2, {1, 2}).toString()));
	REQUIRE(utility::containsElement(inheritanceEdges, TestEdge(1, 1, {1, 2}).toString()));
}

TEST_CASE("HierarchyCache returns inheritance edges for diamond inheritance")
{
	HierarchyCache cache;
	cache.createInheritance(1, 1, 2);
	cache.createInheritance(2, 1, 3);
	cache.createInheritance(3, 2, 4);
	cache.createInheritance(4, 3, 4);
	std::vector<std::string> inheritanceEdges = getSerializedInheritanceEdges(cache, 1, {1, 2, 3, 4});
	REQUIRE(inheritanceEdges.size() == 3);
	REQUIRE(utility::containsElement(inheritanceEdges, TestEdge(1, 2, {1}).toString()));
	REQUIRE(utility::containsElement(inheritanceEdges, TestEdge(1, 3, {2}).toString()));
	REQUIRE(utility::containsElement(inheritanceEdges, TestEdge(1, 4, {1, 2, 3, 4}).toString()));
}
