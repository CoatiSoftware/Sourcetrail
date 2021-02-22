#include "catch.hpp"

#include "HierarchyCache.h"

TEST_CASE("HierarchyCache returns no inheritance edges for empty node set")
{
	HierarchyCache cache;
	cache.createInheritance(1, 1, 2);
	std::vector<std::tuple<Id, Id, std::vector<size_t>>> inheritanceEdges =
		cache.getInheritanceEdgesForNodeId(1, {});
	REQUIRE(inheritanceEdges.size() == 0);
}

TEST_CASE("HierarchyCache returns inheritance edge for non-empty node set")
{
	HierarchyCache cache;
	cache.createInheritance(1, 1, 2);
	std::vector<std::tuple<Id, Id, std::vector<size_t>>> inheritanceEdges =
		cache.getInheritanceEdgesForNodeId(1, {2});
	REQUIRE(inheritanceEdges.size() == 1);
	{
		const std::tuple<Id, Id, std::vector<Id>>& edge = inheritanceEdges[0];
		const Id sourceId = std::get<0>(edge);
		const Id targetId = std::get<1>(edge);
		const std::vector<Id> edgeIds = std::get<2>(edge);
		REQUIRE(sourceId == 1);
		REQUIRE(targetId == 2);
		REQUIRE(edgeIds.size() == 1);
		REQUIRE(edgeIds[0] == 1);
	}
}

TEST_CASE("HierarchyCache skips inheritance for nodes not in set")
{
	HierarchyCache cache;
	cache.createInheritance(1, 1, 2);
	cache.createInheritance(2, 2, 3);
	std::vector<std::tuple<Id, Id, std::vector<size_t>>> inheritanceEdges =
		cache.getInheritanceEdgesForNodeId(1, {3});
	REQUIRE(inheritanceEdges.size() == 1);
	{
		const std::tuple<Id, Id, std::vector<Id>>& edge = inheritanceEdges[0];
		const Id sourceId = std::get<0>(edge);
		const Id targetId = std::get<1>(edge);
		const std::vector<Id> edgeIds = std::get<2>(edge);
		REQUIRE(sourceId == 1);
		REQUIRE(targetId == 3);
		REQUIRE(edgeIds.size() == 2);
		REQUIRE(edgeIds[0] == 1);
		REQUIRE(edgeIds[1] == 2);
	}
}

TEST_CASE("HierarchyCache returns inheritance edges for cyclic inheritance")
{
	HierarchyCache cache;
	cache.createInheritance(1, 1, 2);
	cache.createInheritance(2, 2, 1);
	std::vector<std::tuple<Id, Id, std::vector<size_t>>> inheritanceEdges =
		cache.getInheritanceEdgesForNodeId(1, {1, 2});
	REQUIRE(inheritanceEdges.size() == 2);
	{
		const std::tuple<Id, Id, std::vector<Id>>& edge = inheritanceEdges[0];
		const Id sourceId = std::get<0>(edge);
		const Id targetId = std::get<1>(edge);
		const std::vector<Id> edgeIds = std::get<2>(edge);
		REQUIRE(sourceId == 1);
		REQUIRE(targetId == 2);
		REQUIRE(edgeIds.size() == 1);
		REQUIRE(edgeIds[0] == 1);
	}
	{
		const std::tuple<Id, Id, std::vector<Id>>& edge = inheritanceEdges[1];
		const Id sourceId = std::get<0>(edge);
		const Id targetId = std::get<1>(edge);
		const std::vector<Id> edgeIds = std::get<2>(edge);
		REQUIRE(sourceId == 1);
		REQUIRE(targetId == 1);
		REQUIRE(edgeIds.size() == 2);
		REQUIRE(edgeIds[0] == 1);
		REQUIRE(edgeIds[1] == 2);
	}
}
