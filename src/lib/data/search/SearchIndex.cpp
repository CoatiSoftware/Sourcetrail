#include "data/search/SearchIndex.h"

#include <algorithm>
#include <cctype>

#include "data/search/SearchMatch.h"

std::vector<SearchMatch> SearchIndex::getMatches(
	const SearchResults& searchResults,
	const std::string& query
){
	std::vector<SearchMatch> result;

	for (SearchResultsIterator it = searchResults.begin(); it != searchResults.end(); it++)
	{
		SearchMatch match = it->node->fuzzyMatchData(query, it->parent);
		result.push_back(match);
	}

	return result;
}

SearchIndex::SearchIndex()
	: m_root(nullptr, DELIMITER, 0)
{
}

SearchIndex::~SearchIndex()
{
}

void SearchIndex::clear()
{
	m_root.m_nodes.clear();
}

Id SearchIndex::getWordId(const std::string& word)
{
	return m_dictionary.getWordId(word);
}

const std::string& SearchIndex::getWord(Id wordId) const
{
	return m_dictionary.getWord(wordId);
}

SearchNode* SearchIndex::addNode(std::vector<std::string> nameHierarchy)
{
	std::deque<Id> nameIds;
	for (const std::string& name: nameHierarchy)
	{
		nameIds.push_back(m_dictionary.getWordId(name));
	}
	if (nameIds.size())
	{
		return m_root.addNodeRecursive(&nameIds, m_dictionary).get();
	}

	return nullptr;
}

SearchNode* SearchIndex::getNode(const std::string& fullName) const
{
	std::deque<Id> nameIds = m_dictionary.getWordIdsConst(fullName, DELIMITER);

	if (nameIds.size())
	{
		return m_root.getNodeRecursive(&nameIds).get();
	}

	return nullptr;
}

SearchResults SearchIndex::runFuzzySearch(const std::string& query) const
{
	return m_root.runFuzzySearch(query, true);
}

std::vector<SearchMatch> SearchIndex::runFuzzySearchAndGetMatches(const std::string& query) const
{
	return getMatches(runFuzzySearch(query), query);
}

const std::string SearchIndex::DELIMITER = "::";
