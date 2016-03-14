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

	for (const std::pair<Id, std::shared_ptr<SearchNode>>& p : m_nodes)
	{
		count += p.second->getNodeCount();
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

	for (const std::pair<Id, std::shared_ptr<SearchNode>>& p : m_nodes)
	{
		if (p.second->hasTokenIdsRecursive())
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

const std::map<Id, std::shared_ptr<SearchNode>>& SearchNode::getChildren() const
{
	return m_nodes;
}

SearchResults SearchNode::runFuzzySearch(const std::string& query) const
{
	SearchResults result;

	for (const std::pair<Id, std::shared_ptr<SearchNode>>& p : m_nodes)
	{
		FuzzyMap m = p.second->fuzzyMatchRecursive(query, 0, 0, 0);
		for (const std::pair<size_t, const SearchNode*>& p2 : m)
		{
			addResultsRecursive(&result, p2.first, p2.second, p.second.get());
		}
	}

	return result;
}

SearchResults SearchNode::runFuzzySearchCached(const std::string& query, const SearchResults& searchResults) const
{
	SearchResults result;

	std::set<const SearchNode*> nodes;
	for (const SearchResult& r : searchResults)
	{
		nodes.insert(r.parent);
	}

	for (const SearchNode* n : nodes)
	{
		FuzzyMap m = n->fuzzyMatchRecursive(query, 0, 0, 0);
		for (const std::pair<size_t, const SearchNode*>& p : m)
		{
			addResultsRecursive(&result, p.first, p.second, n);
		}
	}

	return result;
}

SearchResults SearchNode::runFuzzySearchOnSelf(const std::string& query) const
{
	SearchResults result;
	FuzzyMap m = fuzzyMatchRecursive(query, 0, 0, 0);
	for (const std::pair<size_t, const SearchNode*>& p : m)
	{
		addResultsRecursive(&result, p.first, p.second, this);
	}

	return result;
}

void SearchNode::addResultsRecursive(
	SearchResults* results, size_t weight, const SearchNode* node, const SearchNode* parent
) const {
	results->insert(SearchResult(weight, node, parent));

	for (const std::pair<Id, std::shared_ptr<SearchNode>>& p : node->m_nodes)
	{
		addResultsRecursive(results, weight, p.second.get(), parent);
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
		m_nodes.emplace(nameId, node);
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
	std::map<Id, std::shared_ptr<SearchNode>>::iterator it = m_nodes.find(node->getNameId());

	if (it != m_nodes.end())
	{
		m_nodes.erase(it);
	}
}

SearchMatch SearchNode::fuzzyMatchData(const std::string& query, const SearchNode* parent) const
{
	SearchMatch data;
	data.nameHierarchy = getNameHierarchy();
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

	for (const std::pair<Id, std::shared_ptr<SearchNode>>& p : m_nodes)
	{
		FuzzyMap m = p.second->fuzzyMatchRecursive(query, pos, weight, size + m_name.size() + SearchIndex::DELIMITER.size());
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

	size_t ql = query.size();

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

	char last = '\0';
	size_t nl = m_name.size();
	for (size_t i = 0; i < nl; i++)
	{
		char c = m_name[i];
		char next = (i + 1 == nl ? '\0' : m_name[i + 1]);

		if (tolower(query[pos]) == tolower(c))
		{
			weight += std::max<size_t>(100 / sqrt(size + i + 1), 1);

			if (matchCount)
			{
				weight += matchCount * matchCount * 10;
			}


			if (i == 0)
			{
				weight += 20;
			}

			if (last == '_' || next == '_' || next == '\0')
			{
				weight += 20;
			}
			else if ((tolower(c) != c && tolower(next) == next) || (tolower(c) == c && tolower(next) != next))
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

		last = c;
	}

	return std::pair<size_t, size_t>(pos, weight);
}

std::shared_ptr<SearchNode> SearchNode::getChildWithNameId(Id nameId) const
{
	std::map<Id, std::shared_ptr<SearchNode>>::const_iterator it = m_nodes.find(nameId);

	if (it != m_nodes.end())
	{
		return it->second;
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

	for (const std::pair<Id, std::shared_ptr<SearchNode>> p : node->m_nodes)
	{
		ostream << p.second.get();
	}

	return ostream;
}
