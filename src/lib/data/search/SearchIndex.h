#ifndef SEARCH_INDEX_H
#define SEARCH_INDEX_H

#include <map>
#include <memory>
#include <vector>
#include <set>
#include <string>
#include <unordered_set>

#include "utility/types.h"
#include "data/graph/Node.h"
#include "data/NodeTypeSet.h"

struct SearchResult
{
	bool operator<(const SearchResult& other) const
	{
		return score > other.score;
	}

	std::string text;
	std::set<Id> elementIds;
	std::vector<size_t> indices;
	int score;
};

class SearchIndex
{
public:
	SearchIndex();
	virtual ~SearchIndex();

	void addNode(Id id, const std::string& name, NodeTypeSet typeSet = NodeTypeSet::all());
	void finishSetup();
	void clear();

	// maxResultCount == 0 means "no restriction".
	std::vector<SearchResult> search(
		const std::string& query, NodeTypeSet acceptedNodeTypes, size_t maxResultCount, size_t maxBestScoredResultsLength = 0) const;

private:
	struct SearchEdge;

	struct SearchNode
	{
		std::set<Id> elementIds;
		NodeTypeSet containedTypes;
		std::map<char, SearchEdge*> edges;
	};

	struct SearchEdge
	{
		SearchNode* target;
		std::string s;
		std::unordered_set<char> gate;
	};

	struct SearchPath
	{
		std::string text;
		std::vector<size_t> indices;
		SearchNode* node;
	};

	void populateEdgeGate(SearchEdge* e);
	void searchRecursive(const SearchPath& path, const std::string& remainingQuery, NodeTypeSet acceptedNodeTypes,
		std::vector<SearchIndex::SearchPath>* results) const;

	std::multiset<SearchResult> createScoredResults(
		const std::vector<SearchPath>& paths, NodeTypeSet acceptedNodeTypes, size_t maxResultCount) const;

	static SearchResult bestScoredResult(
		SearchResult result, std::map<std::string, SearchResult>* scoresCache, size_t maxBestScoredResultsLength);
	static void bestScoredResultRecursive(
		const std::string& lowerText, const std::vector<size_t>& indices, const size_t lastIndex, const size_t indicesPos,
		std::map<std::string, SearchResult>* scoresCache, SearchResult* result);
	static int scoreText(const std::string& text, const std::vector<size_t>& indices);

public:
	static SearchResult rescoreText(
		const std::string& fulltext,
		const std::string& text,
		const std::vector<size_t>& indices,
		int score,
		size_t maxBestScoredResultsLength);

private:
	std::vector<std::shared_ptr<SearchNode>> m_nodes;
	std::vector<std::shared_ptr<SearchEdge>> m_edges;
	SearchNode* m_root;
};

#endif // SEARCH_INDEX_H
