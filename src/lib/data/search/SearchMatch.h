#ifndef SEARCH_MATCH_H
#define SEARCH_MATCH_H

#include <ostream>
#include <set>
#include <string>
#include <vector>

#include "data/graph/Node.h"
#include "data/NodeTypeSet.h"
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
		COMMAND_ERROR,
		COMMAND_NODE_FILTER
	};

	static void log(const std::vector<SearchMatch>& matches, const std::string& query);

	static std::string getSearchTypeName(SearchType type);
	static std::string searchMatchesToString(const std::vector<SearchMatch>& matches);

	static SearchMatch createCommand(CommandType type);
	static std::vector<SearchMatch> createCommandsForNodeTypes(NodeTypeSet types);
	static std::string getCommandName(CommandType type);

	static const char FULLTEXT_SEARCH_CHARACTER = '?';

	SearchMatch();
	SearchMatch(const std::string& query);

	bool operator<(const SearchMatch& other) const;
	bool operator==(const SearchMatch& other) const;

	size_t getTextSizeForSorting(const std::string* str) const;

	bool isValid() const;
	bool isFilterCommand() const;

	void print(std::ostream& ostream) const;

	std::string getFullName() const;
	std::string getNodeTypeAsUnderscoredString() const;
	std::string getSearchTypeName() const;
	CommandType getCommandType() const;

	std::string name;
	std::vector<Id> tokenIds;

	std::string text;
	std::string subtext;

	NameDelimiterType delimiter;

	std::string typeName;

	NodeType nodeType;
	SearchType searchType;
	std::vector<size_t> indices;

	int score;

	bool hasChildren;
};


#endif // SEARCH_MATCH_H
