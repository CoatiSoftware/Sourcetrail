#ifndef SEARCH_INDEX_H
#define SEARCH_INDEX_H

#include <map>
#include <memory>
#include <vector>
#include <set>
#include <string>
#include <unordered_set>

#include "utility/types.h"

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

	void addNode(Id id, const std::string& name);
	void finishSetup();
	void clear();

	// maxResultCount == 0 means "no restriction".
	std::vector<SearchResult> search(const std::string& query, size_t maxResultCount, size_t maxBestScoredLength = 0) const;

private:
	struct Node;
	struct Edge;

	struct Node
	{
		std::set<Id> elementIds;
		std::map<char, Edge*> edges;
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

	static SearchResult bestScoredResult(
		SearchResult result, std::map<std::string, SearchResult>* scoresCache, size_t maxBestScoredLength);
	static void bestScoredResultRecursive(
		const std::string& lowerText, const std::vector<size_t>& indices, const size_t indicesPos,
		std::map<std::string, SearchResult>* scoresCache, SearchResult* result);
	static int scoreText(const std::string& text, const std::vector<size_t>& indices);

public:
	static SearchResult rescoreText(
		const std::string& fulltext,
		const std::string& text,
		const std::vector<size_t>& indices,
		int score,
		size_t maxBestScoredLength);

private:
	std::vector<std::shared_ptr<Node>> m_nodes;
	std::vector<std::shared_ptr<Edge>> m_edges;
	Node* m_root;
};

#endif // SEARCH_INDEX_H
