#ifndef SEARCH_INDEX_H
#define SEARCH_INDEX_H

#include <ostream>
#include <set>
#include <vector>

#include "utility/text/Dictionary.h"
#include "utility/types.h"

#include "data/search/SearchNode.h"

class SearchIndex
{
public:
	static std::vector<SearchMatch> getMatches(const SearchResults& searchResults, const std::string& query);

	SearchIndex();
	virtual ~SearchIndex();

	void clear();

	size_t getNodeCount() const;

	Id getWordId(const std::string& word);
	const std::string& getWord(Id wordId) const;

	SearchNode* addNode(std::vector<std::string> nameHierarchy);
	SearchNode* getNode(const std::string& fullName) const;
	SearchNode* getNode(const SearchNode* searchNode) const;

	void removeNode(SearchNode* searchNode);
	bool removeNodeIfUnreferencedRecursive(SearchNode* searchNode);

	SearchResults runFuzzySearch(const std::string& query) const;
	std::vector<SearchMatch> runFuzzySearchAndGetMatches(const std::string& query) const;

	static const std::string DELIMITER;

private:
	SearchNode m_root;
	Dictionary m_dictionary;
};

#endif // SEARCH_INDEX_H
