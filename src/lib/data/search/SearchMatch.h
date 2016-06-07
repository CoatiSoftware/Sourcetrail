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
		SEARCH_OPERATOR,
		SEARCH_FULLTEXT
	};

	enum CommandType
	{
		COMMAND_ALL,
		COMMAND_ERROR
	};

	static void log(const std::vector<SearchMatch>& matches, const std::string& query);

	static std::string getSearchTypeName(SearchType type);
	static std::string searchMatchesToString(const std::vector<SearchMatch>& matches);

	static SearchMatch createCommand(CommandType type);
	static std::string getCommandName(CommandType type);

	SearchMatch();
	SearchMatch(const std::string& query);

	bool isValid() const;

	void print(std::ostream& ostream) const;

	std::string getFullName() const;
	std::string getNodeTypeAsString() const;
	std::string getSearchTypeName() const;

	std::string text;
	std::string typeName;
	Node::NodeType nodeType;
	SearchType searchType;
	std::vector<size_t> indices;

	std::vector<NameHierarchy> nameHierarchies;
};


#endif // SEARCH_MATCH_H
