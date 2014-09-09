#include "data/SearchIndex.h"

#include <algorithm>
#include <cctype>

#include "utility/logging/logging.h"
#include "utility/text/Dictionary.h"
#include "utility/utilityString.h"

namespace
{
	bool fncomp(const SearchIndex::SearchNode::FuzzySetPair& lhs, const SearchIndex::SearchNode::FuzzySetPair& rhs)
	{
		if (lhs.first != rhs.first)
		{
			return lhs.first > rhs.first;
		}

		return lhs.second->getFullName() < rhs.second->getFullName();
	}
}

void SearchIndex::SearchMatch::print(std::ostream& ostream) const
{
	ostream << weight << '\t' << node->getFullName() << std::endl << '\t';
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

std::shared_ptr<SearchIndex::SearchNode> SearchIndex::SearchNode::addNodeRecursive(std::deque<Id>* nameIds)
{
	Id nameId = nameIds->front();
	nameIds->pop_front();

	std::shared_ptr<SearchNode> node = getChildWithNameId(nameId);
	if (!node)
	{
		node = std::make_shared<SearchNode>(this, Dictionary::getInstance()->getWord(nameId), nameId);
		m_nodes.insert(node);
	}

	if (nameIds->size())
	{
		return node->addNodeRecursive(nameIds);
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

std::vector<SearchIndex::SearchMatch> SearchIndex::SearchNode::findFuzzyMatches(const std::string& query) const
{
	std::vector<SearchIndex::SearchMatch> result;

	if (!query.size())
	{
		return result;
	}

	// TODO: Currently all matches are added to the ordered set and get compared by their fullName for alphabetical
	// order. This should be avoided e.g. by only returning a subset of the best 100 matches in alphabetical order.
	FuzzySet ordered(&fncomp);
	for (std::shared_ptr<SearchNode> n: m_nodes)
	{
		FuzzyMap m = n->fuzzyMatchRecursive(query, 0, 0, 0);
		ordered.insert(m.begin(), m.end());
	}

	for (FuzzySetIterator it = ordered.begin(); it != ordered.end(); it++)
	{
		SearchMatch match = it->second->fuzzyMatchData(query, this);
		result.push_back(match);

		if (it->first != match.weight)
		{
			LOG_ERROR("Weight between matching and meta data is different.");
		}
	}

	return result;
}

SearchIndex::SearchNode::FuzzyMap SearchIndex::SearchNode::fuzzyMatchRecursive(
	const std::string& query, size_t pos, size_t weight, size_t size) const
{
	FuzzyMap result;

	size_t length = query.size();
	if (pos == length)
	{
		return result;
	}

	std::pair<size_t, size_t> p = fuzzyMatch(query, pos, size);
	pos = p.first;
	weight += p.second;

	if (pos == length)
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
	data.node = this;
	data.weight = 0;

	size_t pos = 0;
	size_t size = 0;

	std::deque<const SearchNode*> nodes = getNodesToParent(parent);
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

SearchIndex::SearchNode* SearchIndex::addNode(const std::string& fullName)
{
	std::deque<Id> nameIds = Dictionary::getInstance()->getWordIds(fullName, DELIMITER);

	if (nameIds.size())
	{
		return m_root.addNodeRecursive(&nameIds).get();
	}

	return nullptr;
}

SearchIndex::SearchNode* SearchIndex::getNode(const std::string& fullName) const
{
	std::deque<Id> nameIds = Dictionary::getInstance()->getWordIds(fullName, DELIMITER);

	if (nameIds.size())
	{
		return m_root.getNodeRecursive(&nameIds).get();
	}

	return nullptr;
}

std::vector<SearchIndex::SearchMatch> SearchIndex::findFuzzyMatches(const std::string& query) const
{
	std::vector<std::string> names = utility::split<std::vector<std::string>>(query, '\"');

	if (names.size() == 3 && names[0].size() == 0)
	{
		SearchNode* node = getNode(names[1]);
		if (!node)
		{
			LOG_ERROR_STREAM(<< "Couldn't find node with name " << names[1] << " in the SearchIndex.");
		}

		return node->findFuzzyMatches(names[2]);
	}

	return m_root.findFuzzyMatches(query);
}

const std::string SearchIndex::DELIMITER = "::";
