#ifndef SEARCH_RESULT_H
#define SEARCH_RESULT_H

#include <set>
#include <stddef.h>

class SearchNode;

struct SearchResult
{
	SearchResult();
	SearchResult(size_t weight, const SearchNode* node, const SearchNode* parent);

	bool operator()(const SearchResult& lhs, const SearchResult& rhs) const;

	size_t weight;
	const SearchNode* node;
	const SearchNode* parent;
};

typedef std::set<SearchResult, SearchResult> SearchResults;
typedef SearchResults::const_iterator SearchResultsIterator;

#endif // SEARCH_RESULT_H
