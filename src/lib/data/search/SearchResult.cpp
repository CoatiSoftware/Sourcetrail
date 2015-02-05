#include "data/search/SearchResult.h"

#include "utility/utilityString.h"

#include "data/search/SearchNode.h"

SearchResult::SearchResult()
{
}

SearchResult::SearchResult(size_t weight, const SearchNode* node, const SearchNode* parent)
	: weight(weight)
	, node(node)
	, parent(parent)
{
}

bool SearchResult::operator()(const SearchResult& lhs, const SearchResult& rhs) const
{
	if (lhs.weight != rhs.weight)
	{
		return lhs.weight > rhs.weight;
	}

	return utility::toLowerCase(lhs.node->getFullName()) < utility::toLowerCase(rhs.node->getFullName());
}
