#ifndef SEARCH_INDEX_H
#define SEARCH_INDEX_H

#include <ostream>
#include <set>
#include <vector>

#include "utility/text/Dictionary.h"
#include "utility/types.h"

#include "data/name/NameHierarchy.h"
#include "data/search/SearchNode.h"

class SearchIndex
{
public:
	static std::vector<SearchMatch> getMatches(const SearchResults& searchResults, const std::string& query);

	SearchIndex();
	virtual ~SearchIndex();

	void clear();

	size_t getNodeCount() const;
	size_t getCharCount() const;
	size_t getWordCount() const;

	Id getWordId(const std::string& word);
	const std::string& getWord(Id wordId) const;

	SearchNode* addNode(NameHierarchy nameHierarchy);
	SearchNode* getNode(const NameHierarchy& nameHierarchy) const;
	SearchNode* getNode(const std::string& fullName) const;
	SearchNode* getNode(const SearchNode* searchNode) const;

	void removeNode(SearchNode* searchNode);
	bool removeNodeIfUnreferencedRecursive(SearchNode* searchNode);

	void addTokenId(SearchNode* node, Id tokenId);
	NameHierarchy getNameHierarchyForTokenId(Id tokenId) const;

	SearchResults runFuzzySearch(const std::string& query) const;
	std::vector<SearchMatch> runFuzzySearchAndGetMatches(const std::string& query) const;

	static const std::string DELIMITER;

private:
	SearchNode m_root;
	Dictionary m_dictionary;

	std::map<Id, SearchNode*> m_tokenIds;

	friend std::ostream& operator<<(std::ostream& ostream, const SearchIndex& index);
};

std::ostream& operator<<(std::ostream& ostream, const SearchIndex& index);

#endif // SEARCH_INDEX_H
