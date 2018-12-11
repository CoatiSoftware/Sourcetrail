#ifndef SEARCH_MATCH_H
#define SEARCH_MATCH_H

#include <ostream>
#include <set>
#include <string>
#include <vector>

#include "Node.h"
#include "types.h"

class NodeTypeSet;

// SearchMatch is used to display the search result in the UI
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
		COMMAND_NODE_FILTER,
		COMMAND_LEGEND
	};

	static void log(const std::vector<SearchMatch>& matches, const std::wstring& query);

	static std::wstring getSearchTypeName(SearchType type);
	static std::wstring searchMatchesToString(const std::vector<SearchMatch>& matches);

	static SearchMatch createCommand(CommandType type);
	static std::vector<SearchMatch> createCommandsForNodeTypes(NodeTypeSet types);
	static std::wstring getCommandName(CommandType type);

	static const wchar_t FULLTEXT_SEARCH_CHARACTER = L'?';

	SearchMatch();
	SearchMatch(const std::wstring& query);

	bool operator<(const SearchMatch& other) const;
	bool operator==(const SearchMatch& other) const;

	size_t getTextSizeForSorting(const std::wstring* str) const;

	bool isValid() const;
	bool isFilterCommand() const;

	void print(std::wostream& ostream) const;

	std::wstring getFullName() const;
	std::wstring getSearchTypeName() const;
	CommandType getCommandType() const;

	std::wstring name;

	std::wstring text;
	std::wstring subtext;

	std::vector<Id> tokenIds;
	std::vector<NameHierarchy> tokenNames;

	std::wstring typeName;

	NodeType nodeType;
	SearchType searchType;
	std::vector<size_t> indices;

	int score = 0;
	bool hasChildren = false;
};


#endif // SEARCH_MATCH_H
