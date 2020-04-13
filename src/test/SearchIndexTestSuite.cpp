#include "catch.hpp"

#include "../lib/data/name/NameHierarchy.h"
#include "../lib/data/search/SearchIndex.h"
#include "../lib/utility/utility.h"

TEST_CASE("search index finds id of element added")
{
	SearchIndex index;
	index.addNode(1, NameHierarchy::deserialize(L"::\tmfoo\tsvoid\tp() const").getQualifiedName());
	index.finishSetup();
	std::vector<SearchResult> results = index.search(L"oo", NodeTypeSet::all(), 0);

	REQUIRE(1 == results.size());
	REQUIRE(1 == results[0].elementIds.size());
	REQUIRE(utility::containsElement<Id>(results[0].elementIds, 1));
}

TEST_CASE("search index finds correct indices for query")
{
	SearchIndex index;
	index.addNode(1, NameHierarchy::deserialize(L"::\tmfoo\tsvoid\tp() const").getQualifiedName());
	index.finishSetup();
	std::vector<SearchResult> results = index.search(L"oo", NodeTypeSet::all(), 0);

	REQUIRE(1 == results.size());
	REQUIRE(2 == results[0].indices.size());
	REQUIRE(1 == results[0].indices[0]);
	REQUIRE(2 == results[0].indices[1]);
}

TEST_CASE("search index finds ids for ambiguous query")
{
	SearchIndex index;
	index.addNode(1, NameHierarchy::deserialize(L"::\tmfor\tsvoid\tp() const").getQualifiedName());
	index.addNode(2, NameHierarchy::deserialize(L"::\tmfos\tsvoid\tp() const").getQualifiedName());
	index.finishSetup();
	std::vector<SearchResult> results = index.search(L"fo", NodeTypeSet::all(), 0);

	REQUIRE(2 == results.size());
	REQUIRE(1 == results[0].elementIds.size());
	REQUIRE(utility::containsElement<Id>(results[0].elementIds, 1));
	REQUIRE(1 == results[1].elementIds.size());
	REQUIRE(utility::containsElement<Id>(results[1].elementIds, 2));
}

TEST_CASE("search index does not find anything after clear")
{
	SearchIndex index;
	index.addNode(1, NameHierarchy::deserialize(L"::\tmfoo\tsvoid\tp() const").getQualifiedName());
	index.finishSetup();
	index.clear();
	std::vector<SearchResult> results = index.search(L"oo", NodeTypeSet::all(), 0);

	REQUIRE(0 == results.size());
}

TEST_CASE("search index does not find all results when max amount is limited")
{
	SearchIndex index;
	index.addNode(1, NameHierarchy::deserialize(L"::\tmfoo1\tsvoid\tp() const").getQualifiedName());
	index.addNode(2, NameHierarchy::deserialize(L"::\tmfoo2\tsvoid\tp() const").getQualifiedName());
	index.finishSetup();
	std::vector<SearchResult> results = index.search(L"oo", NodeTypeSet::all(), 1);

	REQUIRE(1 == results.size());
}

TEST_CASE("search index query is case insensitive")
{
	SearchIndex index;
	index.addNode(1, NameHierarchy::deserialize(L"::\tmfoo1\tsvoid\tp() const").getQualifiedName());
	index.addNode(2, NameHierarchy::deserialize(L"::\tmFOO2\tsvoid\tp() const").getQualifiedName());
	index.finishSetup();
	std::vector<SearchResult> results = index.search(L"oo", NodeTypeSet::all(), 0);

	REQUIRE(2 == results.size());
}

TEST_CASE("search index rates higher on consecutive letters")
{
	SearchIndex index;
	index.addNode(
		1, NameHierarchy::deserialize(L"::\tmoaabbcc\tsvoid\tp() const").getQualifiedName());
	index.addNode(
		2, NameHierarchy::deserialize(L"::\tmocbcabc\tsvoid\tp() const").getQualifiedName());
	index.finishSetup();
	std::vector<SearchResult> results = index.search(L"abc", NodeTypeSet::all(), 0);

	REQUIRE(2 == results.size());
	REQUIRE(L"ocbcabc" == results[0].text);
	REQUIRE(L"oaabbcc" == results[1].text);
}
