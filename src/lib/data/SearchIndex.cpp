#include "data/SearchIndex.h"

#include <algorithm>
#include <cctype>

#include "data/query/QueryCommand.h"
#include "data/query/QueryToken.h"
#include "utility/logging/logging.h"
#include "utility/utilityString.h"

SearchIndex::SearchResult::SearchResult()
{
}

SearchIndex::SearchResult::SearchResult(
	size_t weight,
	const SearchIndex::SearchNode* node,
	const SearchIndex::SearchNode* parent
)
	: weight(weight)
	, node(node)
	, parent(parent)
{
}

bool SearchIndex::SearchResult::operator()(const SearchResult& lhs, const SearchResult& rhs) const
{
	if (lhs.weight != rhs.weight)
	{
		return lhs.weight > rhs.weight;
	}

	return lhs.node->getFullName() < rhs.node->getFullName();
}

void SearchIndex::SearchMatch::print(std::ostream& ostream) const
{
	ostream << weight << '\t' << fullName << std::endl << '\t';
	size_t i = 0;
	for (size_t index : indices)
	{
		while (i < index)
		{
			i++;
			ostream << ' ';
		}
		ostream << '^';
		i++;
	}
	ostream << std::endl;
}

std::string SearchIndex::SearchMatch::encodeForQuery() const
{
	if (!tokenIds.size())
	{
		return QueryCommand::BOUNDARY + fullName + QueryCommand::BOUNDARY;
	}

	std::stringstream ss;
	ss << QueryToken::BOUNDARY << fullName;
	for (Id tokenId : tokenIds)
	{
		ss << QueryToken::DELIMITER << tokenId;
	}
	ss << QueryToken::BOUNDARY;
	return ss.str();
}

SearchIndex::SearchNode::SearchNode(SearchNode* parent, const std::string& name, Id nameId)
	: m_parent(parent)
	, m_name(name)
	, m_nameId(nameId)
{
}

SearchIndex::SearchNode::~SearchNode()
{
}

const std::string& SearchIndex::SearchNode::getName() const
{
	return m_name;
}

std::string SearchIndex::SearchNode::getFullName() const
{
	if (m_parent && m_parent->m_nameId)
	{
		return m_parent->getFullName() + DELIMITER + getName();
	}
	else
	{
		return getName();
	}
}

Id SearchIndex::SearchNode::getNameId() const
{
	return m_nameId;
}

Id SearchIndex::SearchNode::getFirstTokenId() const
{
	if (m_tokenIds.size())
	{
		return *m_tokenIds.begin();
	}

	return 0;
}

const std::set<Id>& SearchIndex::SearchNode::getTokenIds() const
{
	return m_tokenIds;
}

void SearchIndex::SearchNode::addTokenId(Id tokenId)
{
	m_tokenIds.insert(tokenId);
}

SearchIndex::SearchNode* SearchIndex::SearchNode::getParent() const
{
	if (m_parent && m_parent->m_nameId)
	{
		return m_parent;
	}
	return nullptr;
}

std::deque<SearchIndex::SearchNode*> SearchIndex::SearchNode::getParentsWithoutTokenId()
{
	std::deque<SearchNode*> nodes;

	SearchNode* node = this;
	while (node->m_nameId && !node->m_tokenIds.size())
	{
		nodes.push_front(node);
		node = node->m_parent;
	}

	return nodes;
}

const std::set<std::shared_ptr<SearchIndex::SearchNode>>& SearchIndex::SearchNode::getChildren() const
{
	return m_nodes;
}

SearchIndex::SearchResults SearchIndex::SearchNode::runFuzzySearch(const std::string& query, bool recursive) const
{
	SearchResults result;

	if (recursive)
	{
		for (std::shared_ptr<SearchNode> n: m_nodes)
		{
			FuzzyMap m = n->fuzzyMatchRecursive(query, 0, 0, 0);
			for (const std::pair<size_t, const SearchNode*>& p : m)
			{
				result.insert(SearchResult(p.first, p.second, this));
			}
		}
	}
	else
	{
		std::pair<size_t, size_t> p = fuzzyMatch(query, 0, 0);
		size_t pos = p.first;
		size_t weight = p.second;

		if (pos == query.size())
		{
			result.insert(SearchResult(weight, this, this));
		}
	}

	// TODO: Currently all matches are added to the ordered set and get compared by their fullName for alphabetical
	// order. This could be improved by limiting the number of items to e.g. 100.
	return result;
}

std::shared_ptr<SearchIndex::SearchNode> SearchIndex::SearchNode::addNodeRecursive(
	std::deque<Id>* nameIds, const Dictionary& dictionary
){
	Id nameId = nameIds->front();
	nameIds->pop_front();

	std::shared_ptr<SearchNode> node = getChildWithNameId(nameId);
	if (!node)
	{
		node = std::make_shared<SearchNode>(this, dictionary.getWord(nameId), nameId);
		m_nodes.insert(node);
	}

	if (nameIds->size() > 0)
	{
		return node->addNodeRecursive(nameIds, dictionary);
	}

	return node;
}

std::shared_ptr<SearchIndex::SearchNode> SearchIndex::SearchNode::getNodeRecursive(std::deque<Id>* nameIds) const
{
	Id nameId = nameIds->front();
	nameIds->pop_front();

	std::shared_ptr<SearchNode> node = getChildWithNameId(nameId);
	if (node)
	{
		if (!nameIds->size())
		{
			return node;
		}

		return node->getNodeRecursive(nameIds);
	}

	return nullptr;
}

SearchIndex::SearchNode::FuzzyMap SearchIndex::SearchNode::fuzzyMatchRecursive(
	const std::string& query, size_t pos, size_t weight, size_t size) const
{
	FuzzyMap result;
	std::pair<size_t, size_t> p = fuzzyMatch(query, pos, size);
	pos = p.first;
	weight += p.second;

	if (pos == query.size())
	{
		result.emplace(weight, this);
		return result;
	}

	for (std::shared_ptr<SearchNode> n: m_nodes)
	{
		FuzzyMap m = n->fuzzyMatchRecursive(query, pos, weight, size + m_name.size() + SearchIndex::DELIMITER.size());
		result.insert(m.begin(), m.end());
	}

	return result;
}

std::pair<size_t, size_t> SearchIndex::SearchNode::fuzzyMatch(
	const std::string query, size_t start, size_t size, std::vector<size_t>* indices) const
{
	size_t pos = start;
	size_t weight = 0;
	size_t matchCount = 0;
	char lastChar = '\0';

	size_t ql = query.size();
	size_t ml = m_name.size();

	if (!query.size())
	{
		return std::pair<size_t, size_t>(pos, weight);
	}

	if (query[pos] == ':')
	{
		pos++;
		if (indices && size >= 2)
		{
			indices->push_back(size - 2);
		}

		if (pos < ql && query[pos] == ':')
		{
			pos++;
			if (indices && size >= 1)
			{
				indices->push_back(size - 1);
			}
		}
	}

	for (size_t i = 0; i < ml; i++)
	{
		char c = m_name[i];
		if (tolower(query[pos]) == tolower(c))
		{
			weight += std::max<size_t>(100 - size - i, 1);
			if (matchCount)
			{
				weight += matchCount * 10;
			}
			else if (i == 0 || lastChar == '_' || tolower(c) != c)
			{
				weight += 20;
			}
			matchCount++;

			pos++;
			if (indices)
			{
				indices->push_back(size + i);
			}

			if (pos == ql || query[pos] == ':')
			{
				break;
			}
		}
		else
		{
			matchCount = 0;
		}

		lastChar = c;
	}

	return std::pair<size_t, size_t>(pos, weight);
}

SearchIndex::SearchMatch SearchIndex::SearchNode::fuzzyMatchData(const std::string& query, const SearchNode* parent) const
{
	SearchMatch data;
	data.fullName = getFullName();
	data.tokenIds = m_tokenIds;
	data.weight = 0;

	size_t pos = 0;
	size_t size = 0;

	std::deque<const SearchNode*> nodes = getNodesToParent(parent);
	if (!nodes.size())
	{
		nodes.push_back(this);
	}

	for (const SearchNode* node : nodes)
	{
		std::pair<size_t, size_t> p = node->fuzzyMatch(query, pos, size, &data.indices);
		pos = p.first;
		data.weight += p.second;
		size += node->m_name.size() + SearchIndex::DELIMITER.size();
	}

	return data;
}

std::shared_ptr<SearchIndex::SearchNode> SearchIndex::SearchNode::getChildWithNameId(Id nameId) const
{
	for (std::shared_ptr<SearchNode> n: m_nodes)
	{
		if (n->m_nameId == nameId)
		{
			return n;
		}
	}

	return nullptr;
}

std::deque<const SearchIndex::SearchNode*> SearchIndex::SearchNode::getNodesToParent(const SearchNode* parent) const
{
	std::deque<const SearchIndex::SearchNode*> nodes;

	const SearchNode* node = this;
	while (node->m_nameId && node != parent)
	{
		nodes.push_front(node);
		node = node->m_parent;
	}

	return nodes;
}


std::vector<SearchIndex::SearchMatch> SearchIndex::getMatches(
	const SearchIndex::SearchResults& searchResults,
	const std::string& query
){
	std::vector<SearchMatch> result;

	for (SearchResultsIterator it = searchResults.begin(); it != searchResults.end(); it++)
	{
		SearchMatch match = it->node->fuzzyMatchData(query, it->parent);
		result.push_back(match);

		if (it->weight != match.weight)
		{
			LOG_ERROR("Weight between matching and meta data is different.");
		}
	}

	return result;
}

void SearchIndex::logMatches(const std::vector<SearchIndex::SearchMatch>& matches, const std::string& query)
{
	std::stringstream ss;
	ss << std::endl << matches.size() << " matches for \"" << query << "\":" << std::endl;
	for (const SearchIndex::SearchMatch& match : matches)
	{
		match.print(ss);
	}
	LOG_INFO(ss.str());
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

SearchIndex::SearchNode* SearchIndex::addNode(std::vector<std::string> nameHierarchy)
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

SearchIndex::SearchNode* SearchIndex::getNode(const std::string& fullName) const
{
	std::deque<Id> nameIds = m_dictionary.getWordIdsConst(fullName, DELIMITER);

	if (nameIds.size())
	{
		return m_root.getNodeRecursive(&nameIds).get();
	}

	return nullptr;
}

SearchIndex::SearchResults SearchIndex::runFuzzySearch(const std::string& query) const
{
	return m_root.runFuzzySearch(query, true);
}

std::vector<SearchIndex::SearchMatch> SearchIndex::runFuzzySearchAndGetMatches(const std::string& query) const
{
	return getMatches(runFuzzySearch(query), query);
}

const std::string SearchIndex::DELIMITER = "::";
