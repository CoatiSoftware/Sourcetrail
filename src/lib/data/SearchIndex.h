#ifndef SEARCH_INDEX_H
#define SEARCH_INDEX_H

#include <deque>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <vector>

#include "utility/types.h"

class SearchIndex
{
public:
	class SearchNode;

	struct SearchMatch
	{
		void print(std::ostream& ostream) const;

		const SearchIndex::SearchNode* node;
		std::vector<size_t> indices;
		size_t weight;
	};

	class SearchNode
	{
	public:
		typedef std::multimap<size_t, const SearchIndex::SearchNode*> FuzzyMap;
		typedef FuzzyMap::const_iterator FuzzyMapIterator;

		typedef std::pair<size_t, const SearchIndex::SearchNode*> FuzzySetPair;
		typedef std::multiset<FuzzySetPair, bool(*)(const FuzzySetPair&, const FuzzySetPair&)> FuzzySet;
		typedef FuzzySet::const_iterator FuzzySetIterator;

		SearchNode(SearchNode* parent, const std::string& name, Id nameId);
		~SearchNode();

		const std::string& getName() const;
		std::string getFullName() const;

		Id getNameId() const;

		Id getFirstTokenId() const;
		const std::set<Id>& getTokenIds() const;
		void addTokenId(Id tokenId);

		SearchNode* getParent() const;
		std::deque<SearchIndex::SearchNode*> getParentsWithoutTokenId();

	private:
		// Accessed by SearchIndex
		std::shared_ptr<SearchNode> addNodeRecursive(std::deque<Id>* nameIds);
		std::shared_ptr<SearchNode> getNodeRecursive(std::deque<Id>* nameIds) const;
		std::vector<SearchIndex::SearchMatch> findFuzzyMatches(const std::string& query) const;

		friend class SearchIndex;

		FuzzyMap fuzzyMatchRecursive(const std::string& query, size_t pos, size_t weight, size_t size) const;
		std::pair<size_t, size_t> fuzzyMatch(
			const std::string query, size_t start, size_t size, std::vector<size_t>* indices = nullptr) const;
		SearchMatch fuzzyMatchData(const std::string& query, const SearchNode* parent) const;

		std::shared_ptr<SearchIndex::SearchNode> getChildWithNameId(Id nameId) const;
		std::deque<const SearchNode*> getNodesToParent(const SearchNode* parent) const;

		std::set<std::shared_ptr<SearchNode>> m_nodes;
		SearchNode* m_parent;

		std::set<Id> m_tokenIds;

		const std::string& m_name;
		const Id m_nameId;
	};

	static void logMatches(const std::vector<SearchIndex::SearchMatch>& matches, const std::string& query);

	SearchIndex();
	virtual ~SearchIndex();

	void clear();

	SearchNode* addNode(const std::string& fullName);
	SearchNode* getNode(const std::string& fullName) const;

	std::vector<SearchIndex::SearchMatch> findFuzzyMatches(const std::string& query) const;

	static const std::string DELIMITER;

private:
	SearchNode m_root;
};

#endif // SEARCH_INDEX_H
