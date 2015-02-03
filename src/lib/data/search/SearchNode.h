#ifndef SEARCH_NODE_H
#define SEARCH_NODE_H

#include <deque>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "utility/types.h"

#include "data/search/SearchResult.h"

class Dictionary;
class SearchIndex;
struct SearchMatch;

class SearchNode
{
public:
	SearchNode(SearchNode* parent, const std::string& name, Id nameId);
	~SearchNode();

	const std::string& getName() const;
	std::vector<std::string> getNameHierarchy() const;
	std::string getFullName() const;

	Id getNameId() const;

	Id getFirstTokenId() const;
	const std::set<Id>& getTokenIds() const;
	void addTokenId(Id tokenId);

	SearchNode* getParent() const;
	std::deque<SearchNode*> getParentsWithoutTokenId();

	const std::set<std::shared_ptr<SearchNode>>& getChildren() const;

	SearchResults runFuzzySearch(const std::string& query) const;
	SearchResults runFuzzySearchOnSelf(const std::string& query) const;

	void addResultsRecursive(SearchResults& result, size_t weight, const SearchNode* node) const;

private:
	typedef std::multimap<size_t, const SearchNode*> FuzzyMap;
	typedef FuzzyMap::const_iterator FuzzyMapIterator;


	// Accessed by SearchIndex
	std::shared_ptr<SearchNode> addNodeRecursive(std::deque<Id>* nameIds, const Dictionary& dictionary);
	std::shared_ptr<SearchNode> getNodeRecursive(std::deque<Id>* nameIds) const;

	SearchMatch fuzzyMatchData(const std::string& query, const SearchNode* parent) const;

	friend class SearchIndex;

	FuzzyMap fuzzyMatchRecursive(const std::string& query, size_t pos, size_t weight, size_t size) const;
	std::pair<size_t, size_t> fuzzyMatch(
		const std::string query, size_t start, size_t size, std::vector<size_t>* indices = nullptr) const;

	std::shared_ptr<SearchNode> getChildWithNameId(Id nameId) const;
	std::deque<const SearchNode*> getNodesToParent(const SearchNode* parent) const;

	std::set<std::shared_ptr<SearchNode>> m_nodes;
	SearchNode* m_parent;

	std::set<Id> m_tokenIds;

	const std::string& m_name;
	const Id m_nameId;
};

#endif // SEARCH_NODE_H
