#include "data/search/SearchIndex.h"

#include <algorithm>
#include <cctype>

#include "utility/logging/logging.h"

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

size_t SearchIndex::getNodeCount() const
{
	return m_root.getNodeCount() - 1;
}

Id SearchIndex::getWordId(const std::string& word)
{
	return m_dictionary.getWordId(word);
}

const std::string& SearchIndex::getWord(Id wordId) const
{
	return m_dictionary.getWord(wordId);
}

SearchNode* SearchIndex::addNode(NameHierarchy nameHierarchy)
{
	std::deque<Id> nameIds;
	for (size_t i = 0; i < nameHierarchy.size(); i++)
	{
		nameIds.push_back(m_dictionary.getWordId(nameHierarchy[i]->getFullName()));
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

SearchNode* SearchIndex::getNode(const SearchNode* searchNode) const
{
	std::deque<Id> nameIds = searchNode->getNameIdsRecursive();

	if (nameIds.size())
	{
		return m_root.getNodeRecursive(&nameIds).get();
	}

	return nullptr;
}

void SearchIndex::removeNode(SearchNode* searchNode)
{
	SearchNode* parent = searchNode->getParent();

	if (!parent)
	{
		LOG_ERROR_STREAM(<< "SearchNode to be removed has no parent: " << searchNode->getFullName());
		return;
	}

	parent->removeSearchNode(searchNode);
}

bool SearchIndex::removeNodeIfUnreferencedRecursive(SearchNode* searchNode)
{
	if (!searchNode->hasTokenIdsRecursive())
	{
		SearchNode* parent = searchNode->getParent();

		removeNode(searchNode);

		if (parent && parent != &m_root)
		{
			removeNodeIfUnreferencedRecursive(parent);
		}

		return true;
	}

	return false;
}

SearchResults SearchIndex::runFuzzySearch(const std::string& query) const
{
	return m_root.runFuzzySearch(query);
}

std::vector<SearchMatch> SearchIndex::runFuzzySearchAndGetMatches(const std::string& query) const
{
	return getMatches(runFuzzySearch(query), query);
}

const std::string SearchIndex::DELIMITER = "::";
