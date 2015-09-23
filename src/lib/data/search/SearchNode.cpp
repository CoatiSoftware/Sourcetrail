#include "data/search/SearchNode.h"

#include <algorithm>

#include "utility/text/Dictionary.h"

#include "data/name/NameHierarchy.h"
#include "data/search/SearchIndex.h"
#include "data/search/SearchMatch.h"
#include "data/search/SearchResult.h"

SearchNode::SearchNode(SearchNode* parent, const std::string& name, Id nameId)
	: m_parent(parent)
	, m_name(name)
	, m_nameId(nameId)
{
}

SearchNode::~SearchNode()
{
}

size_t SearchNode::getNodeCount() const
{
	size_t count = 1;

	for (std::shared_ptr<SearchNode> n: m_nodes)
	{
		count += n->getNodeCount();
	}

	return count;
}

const std::string& SearchNode::getName() const
{
	return m_name;
}

NameHierarchy SearchNode::getNameHierarchy() const
{
	NameHierarchy nameHierarchy;
	const SearchNode* parent = getParent();
	if (parent && parent->m_nameId)
	{
		nameHierarchy = parent->getNameHierarchy();
	}
	nameHierarchy.push(std::make_shared<NameElement>(getName()));
	return nameHierarchy;
}

std::string SearchNode::getFullName() const
{
	if (m_parent && m_parent->m_nameId)
	{
		return m_parent->getFullName() + SearchIndex::DELIMITER + getName();
	}
	else
	{
		return getName();
	}
}

Id SearchNode::getNameId() const
{
	return m_nameId;
}

std::deque<Id> SearchNode::getNameIdsRecursive() const
{
	std::deque<Id> ids;

	ids.push_front(m_nameId);

	SearchNode* parent = m_parent;
	while (parent && parent->m_nameId)
	{
		ids.push_front(parent->getNameId());
		parent = parent->getParent();
	}

	return ids;
}

Id SearchNode::getFirstTokenId() const
{
	if (m_tokenIds.size())
	{
		return *m_tokenIds.begin();
	}

	return 0;
}

const std::set<Id>& SearchNode::getTokenIds() const
{
	return m_tokenIds;
}

bool SearchNode::hasTokenIdsRecursive() const
{
	if (m_tokenIds.size())
	{
		return true;
	}

	for (std::shared_ptr<SearchNode> n: m_nodes)
	{
		if (n->hasTokenIdsRecursive())
		{
			return true;
		}
	}

	return false;
}

void SearchNode::addTokenId(Id tokenId)
{
	m_tokenIds.insert(tokenId);
}

void SearchNode::removeTokenId(Id tokenId)
{
	m_tokenIds.erase(tokenId);
}

SearchNode* SearchNode::getParent() const
{
	if (m_parent)
	{
		return m_parent;
	}
	return nullptr;
}

std::deque<SearchNode*> SearchNode::getParentsWithoutTokenId()
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

const std::set<std::shared_ptr<SearchNode>>& SearchNode::getChildren() const
{
	return m_nodes;
}

SearchResults SearchNode::runFuzzySearch(const std::string& query) const
{
	SearchResults result;

	for (std::shared_ptr<SearchNode> n: m_nodes)
	{
		FuzzyMap m = n->fuzzyMatchRecursive(query, 0, 0, 0);
		for (const std::pair<size_t, const SearchNode*>& p : m)
		{
			addResultsRecursive(&result, p.first, p.second);
		}
	}

	// TODO: Currently all matches are added to the ordered set and get compared by their fullName for alphabetical
	// order. This could be improved by limiting the number of items to e.g. 100.
	return result;
}

SearchResults SearchNode::runFuzzySearchOnSelf(const std::string& query) const
{
	SearchResults result;
	FuzzyMap m = fuzzyMatchRecursive(query, 0, 0, 0);
	for (const std::pair<size_t, const SearchNode*>& p : m)
	{
		addResultsRecursive(&result, p.first, p.second);
	}

	// TODO: Currently all matches are added to the ordered set and get compared by their fullName for alphabetical
	// order. This could be improved by limiting the number of items to e.g. 100.
	return result;
}

void SearchNode::addResults(SearchResults* results, size_t weight, const SearchNode* node) const
{
	results->insert(SearchResult(weight, node, this));
}

void SearchNode::addResultsRecursive(SearchResults* results, size_t weight, const SearchNode* node) const
{
	addResults(results, weight, node);

	for (std::shared_ptr<SearchNode> n: node->m_nodes)
	{
		addResultsRecursive(results, weight, n.get());
	}
}

std::shared_ptr<SearchNode> SearchNode::addNodeRecursive(
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

std::shared_ptr<SearchNode> SearchNode::getNodeRecursive(std::deque<Id>* nameIds) const
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

void SearchNode::removeSearchNode(SearchNode* node)
{
	for (std::set<std::shared_ptr<SearchNode>>::iterator it = m_nodes.begin(); it != m_nodes.end(); it++)
	{
		if ((*it)->m_nameId == node->m_nameId)
		{
			m_nodes.erase(it);
			return;
		}
	}
}

SearchMatch SearchNode::fuzzyMatchData(const std::string& query, const SearchNode* parent) const
{
	SearchMatch data;
	data.fullName = getFullName();
	data.tokenIds = m_tokenIds;
	data.weight = 0;

	size_t pos = 0;
	size_t size = 0;

	const SearchNode* root = parent;
	std::deque<const SearchNode*> nodes = getNodesToParent(parent);
	if (!nodes.size())
	{
		nodes.push_back(this);
		root = root->getParent();
	}

	while (root && root->getNameId())
	{
		size += root->getName().size() + SearchIndex::DELIMITER.size();
		root = root->getParent();
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

SearchNode::FuzzyMap SearchNode::fuzzyMatchRecursive(
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

std::pair<size_t, size_t> SearchNode::fuzzyMatch(
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
			weight += std::max<size_t>(100 / sqrt(size + i + 1), 1);
			if (matchCount)
			{
				weight += matchCount * matchCount * 10;
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

std::shared_ptr<SearchNode> SearchNode::getChildWithNameId(Id nameId) const
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

std::deque<const SearchNode*> SearchNode::getNodesToParent(const SearchNode* parent) const
{
	std::deque<const SearchNode*> nodes;

	const SearchNode* node = this;
	while (node->m_nameId && node != parent)
	{
		nodes.push_front(node);
		node = node->m_parent;
	}

	return nodes;
}

std::ostream& operator<<(std::ostream& ostream, const SearchNode* node)
{
	ostream << node->m_name;

	for (Id tokenId : node->m_tokenIds)
	{
		ostream << ' ' << tokenId;
	}

	ostream << '\n';

	for (const std::shared_ptr<SearchNode> n : node->m_nodes)
	{
		ostream << n.get();
	}

	return ostream;
}
