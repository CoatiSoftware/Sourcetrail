#ifndef SEARCH_MATCH_H
#define SEARCH_MATCH_H

#include <ostream>
#include <set>
#include <string>
#include <vector>

#include "data/graph/Node.h"
#include "utility/types.h"

struct SearchMatch
{
	enum SearchType
	{
		SEARCH_NONE,
		SEARCH_TOKEN,
		SEARCH_COMMAND,
		SEARCH_OPERATOR
	};

	static void log(const std::vector<SearchMatch>& matches, const std::string& query);

	static std::string getSearchTypeName(SearchType type);
	static std::string searchMatchesToString(const std::vector<SearchMatch>& matches);

	SearchMatch();
	SearchMatch(const std::string& query);

	bool isValid() const;

	void print(std::ostream& ostream) const;

	std::string getFullName() const;
	std::string getNodeTypeAsString() const;
	std::string getSearchTypeName() const;

	NameHierarchy nameHierarchy;
	std::string typeName;

	Node::NodeType nodeType;
	SearchType searchType;

	std::set<Id> tokenIds;

	std::vector<size_t> indices;
	size_t weight;
};


#endif // SEARCH_MATCH_H
