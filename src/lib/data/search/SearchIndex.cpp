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
	m_dictionary.clear();
	m_tokenIds.clear();
}

size_t SearchIndex::getNodeCount() const
{
	return m_root.getNodeCount() - 1;
}

size_t SearchIndex::getCharCount() const
{
	return m_dictionary.getCharCount();
}

size_t SearchIndex::getWordCount() const
{
	return m_dictionary.getWordCount();
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

SearchNode* SearchIndex::getNode(const NameHierarchy& nameHierarchy) const
{
	std::deque<Id> nameIds = m_dictionary.getWordIdsConst(nameHierarchy);

	if (nameIds.size())
	{
		return m_root.getNodeRecursive(&nameIds).get();
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

void SearchIndex::addTokenId(SearchNode* node, Id tokenId)
{
	node->addTokenId(tokenId);
	m_tokenIds.emplace(tokenId, node);
}

NameHierarchy SearchIndex::getNameHierarchyForTokenId(Id tokenId) const
{
	std::map<Id, SearchNode*>::const_iterator it = m_tokenIds.find(tokenId);

	if (it != m_tokenIds.end())
	{
		SearchNode* node = it->second;
		return node->getNameHierarchy();
	}

	return NameHierarchy();
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

std::ostream& operator<<(std::ostream& ostream, const SearchIndex& index)
{
	ostream << "SearchIndex:\n";
	ostream << &index.m_root;
	return ostream;
}
