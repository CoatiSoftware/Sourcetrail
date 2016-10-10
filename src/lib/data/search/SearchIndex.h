#ifndef SEARCH_INDEX_H
#define SEARCH_INDEX_H

#include <map>
#include <memory>
#include <vector>
#include <set>
#include <string>
#include <unordered_set>

#include "data/name/NameHierarchy.h"
#include "utility/types.h"

struct SearchResult
{
	std::string text;
	std::set<Id> elementIds;
	std::vector<size_t> indices;
	int score;

	bool operator<(const SearchResult& other) const
	{
		return score > other.score;
	}
};

class SearchIndex
{
public:
	SearchIndex();
	virtual ~SearchIndex();

	void addNode(Id id, const NameHierarchy& nameHierarchy);
	void finishSetup();
	void clear();

	// maxResultCount == 0 means "no restriction".
	std::vector<SearchResult> search(const std::string& query, size_t maxResultCount) const;

private:
	struct Node;
	struct Edge;

	struct Node
	{
		std::set<Id> elementIds;
		std::vector<Edge*> edges;
	};

	struct Edge
	{
		Node* target;
		std::string s;
		std::unordered_set<char> gate;
	};

	struct Path
	{
		std::string text;
		std::vector<size_t> indices;
		Node* node;
	};

	void populateEdgeGate(Edge* e);
	void searchRecursive(const Path& path, const std::string& remainingQuery, std::vector<SearchIndex::Path>* results) const;

	std::multiset<SearchResult> createScoredResults(const std::vector<Path>& paths, size_t maxResultCount) const;
	SearchResult bestScoredResult(SearchResult result, std::map<std::string, SearchResult>* scoresCache) const;
	void bestScoredResultRecursive(
		const std::string& lowerText, const std::vector<size_t>& indices, size_t indicesPos,
		std::map<std::string, SearchResult>* scoresCache, SearchResult* result) const;
	int score(const std::string& text, const std::vector<size_t>& indices) const;

	std::vector<std::shared_ptr<Node>> m_nodes;
	std::vector<std::shared_ptr<Edge>> m_edges;
	Node* m_root;
};

#endif // SEARCH_INDEX_H
