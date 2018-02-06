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

// SearchResult is only used as an internal type in the SearchIndex and the PersistentStorage
struct SearchResult
{
	bool operator<(const SearchResult& other) const
	{
		return score > other.score;
	}

	std::wstring text;
	std::set<Id> elementIds;
	std::vector<size_t> indices;
	int score;
};

class SearchIndex
{
public:
	SearchIndex();
	virtual ~SearchIndex();

	void addNode(Id id, const std::wstring& name, NodeTypeSet typeSet = NodeTypeSet::all());
	void finishSetup();
	void clear();

	// maxResultCount == 0 means "no restriction".
	std::vector<SearchResult> search(
		const std::wstring& query, NodeTypeSet acceptedNodeTypes, size_t maxResultCount, size_t maxBestScoredResultsLength = 0) const;

private:
	struct SearchEdge;

	struct SearchNode
	{
		std::set<Id> elementIds;
		NodeTypeSet containedTypes;
		std::map<wchar_t, SearchEdge*> edges;
	};

	struct SearchEdge
	{
		SearchNode* target;
		std::wstring s;
		std::unordered_set<wchar_t> gate;
	};

	struct SearchPath
	{
		std::wstring text;
		std::vector<size_t> indices;
		SearchNode* node;
	};

	void populateEdgeGate(SearchEdge* e);
	void searchRecursive(const SearchPath& path, const std::wstring& remainingQuery, NodeTypeSet acceptedNodeTypes,
		std::vector<SearchIndex::SearchPath>* results) const;

	std::multiset<SearchResult> createScoredResults(
		const std::vector<SearchPath>& paths, NodeTypeSet acceptedNodeTypes, size_t maxResultCount) const;

	static SearchResult bestScoredResult(
		SearchResult result, std::map<std::wstring, SearchResult>* scoresCache, size_t maxBestScoredResultsLength);
	static void bestScoredResultRecursive(
		const std::wstring& lowerText, const std::vector<size_t>& indices, const size_t lastIndex, const size_t indicesPos,
		std::map<std::wstring, SearchResult>* scoresCache, SearchResult* result);
	static int scoreText(const std::wstring& text, const std::vector<size_t>& indices);

public:
	static SearchResult rescoreText(
		const std::wstring& fulltext,
		const std::wstring& text,
		const std::vector<size_t>& indices,
		int score,
		size_t maxBestScoredResultsLength);

	static bool isNoLetter(const wchar_t c);

private:
	std::vector<std::shared_ptr<SearchNode>> m_nodes;
	std::vector<std::shared_ptr<SearchEdge>> m_edges;
	SearchNode* m_root;
};

#endif // SEARCH_INDEX_H
