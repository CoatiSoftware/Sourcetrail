#include "data/search/SearchMatch.h"

#include <sstream>

#include "utility/logging/logging.h"

void SearchMatch::log(const std::vector<SearchMatch>& matches, const std::string& query)
{
	std::stringstream ss;
	ss << std::endl << matches.size() << " matches for \"" << query << "\":" << std::endl;

	for (const SearchMatch& match : matches)
	{
		match.print(ss);
	}

	LOG_INFO(ss.str());
}

std::string SearchMatch::getSearchTypeName(SearchType type)
{
	switch (type)
	{
	case SEARCH_NONE:
		return "none";
	case SEARCH_TOKEN:
		return "token";
	case SEARCH_COMMAND:
		return "command";
	case SEARCH_OPERATOR:
		return "operator";
	case SEARCH_FULLTEXT:
		return "fulltext";
	}
}

std::string SearchMatch::searchMatchesToString(const std::vector<SearchMatch>& matches)
{
	std::stringstream ss;

	for (size_t i = 0; i < matches.size(); i++)
	{
		ss << '@' << matches[i].getFullName();
	}

	return ss.str();
}

SearchMatch SearchMatch::createCommand(CommandType type)
{
	SearchMatch match;
	match.name = getCommandName(type);
	match.text = match.name;
	match.typeName = "command";
	match.searchType = SEARCH_COMMAND;
	return match;
}

std::vector<SearchMatch> SearchMatch::createCommandsForNodeTypes(NodeTypeSet types)
{
	std::vector<SearchMatch> matches;

	for (const NodeType& type: types.getNodeTypes())
	{
		SearchMatch match;
		match.name = type.getReadableTypeString();
		match.text = match.name;
		match.typeName = "filter";
		match.searchType = SEARCH_COMMAND;
		match.nodeType = type;
		matches.push_back(match);
	}

	return matches;
}

std::string SearchMatch::getCommandName(CommandType type)
{
	switch (type)
	{
	case COMMAND_ALL:
		return "overview";
	case COMMAND_ERROR:
		return "error";
	case COMMAND_NODE_FILTER:
		return "node_filter";
	}

	return "none";
}

SearchMatch::SearchMatch()
	: typeName("")
	, nodeType(NodeType::NODE_SYMBOL)
	, searchType(SEARCH_NONE)
	, hasChildren(false)
{
}

SearchMatch::SearchMatch(const std::string& query)
	: name(query)
	, text(query)
	, typeName("")
	, nodeType(NodeType::NODE_SYMBOL)
	, searchType(SEARCH_NONE)
	, hasChildren(false)
{
}


bool SearchMatch::operator<(const SearchMatch& other) const
{
	// score
	if (score > other.score)
	{
		return true;
	}
	else if (score < other.score)
	{
		return false;
	}

	const std::string* str = &text;
	const std::string* otherStr = &other.text;
	if (*str == *otherStr)
	{
		str = &name;
		otherStr = &other.name;
	}

	size_t size = getTextSizeForSorting(str);
	size_t otherSize = other.getTextSizeForSorting(otherStr);

	// text size
	if (size < otherSize)
	{
		return true;
	}
	else if (size > otherSize)
	{
		return false;
	}
	else if (str->size() < otherStr->size())
	{
		return true;
	}
	else if (str->size() > otherStr->size())
	{
		return false;
	}

	// lower case
	for (size_t i = 0; i < str->size(); i++)
	{
		if (tolower(str->at(i)) != tolower(otherStr->at(i)))
		{
			return tolower(str->at(i)) < tolower(otherStr->at(i));
		}
		else
		{
			// alphabetical
			if (str->at(i) < otherStr->at(i))
			{
				return true;
			}
			else if (str->at(i) > otherStr->at(i))
			{
				return false;
			}
		}
	}

	return false;
}

bool SearchMatch::operator==(const SearchMatch& other) const
{
	return text == other.text && searchType == other.searchType;
}

size_t SearchMatch::getTextSizeForSorting(const std::string* str) const
{
	// check if templated symbol and only use size up to template stuff
	size_t pos = str->find('<');
	if (pos != std::string::npos)
	{
		return pos;
	}

	return str->size();
}

bool SearchMatch::isValid() const
{
	return searchType != SEARCH_NONE;
}

bool SearchMatch::isFilterCommand() const
{
	return searchType == SEARCH_COMMAND && getCommandType() == COMMAND_NODE_FILTER;
}

void SearchMatch::print(std::ostream& ostream) const
{
	ostream << name << std::endl << '\t';
	size_t i = 0;
	for (size_t index : indices)
	{
		while (i < index)
		{
			i++;
			ostream << ' ';
		}
		ostream << '^';
		i++;
	}
	ostream << std::endl;
}

std::string SearchMatch::getFullName() const
{
	if (searchType == SEARCH_TOKEN && nodeType.isFile())
	{
		return text;
	}

	return name;
}

std::string SearchMatch::getNodeTypeAsUnderscoredString() const
{
	return nodeType.getUnderscoredTypeString();
}

std::string SearchMatch::getSearchTypeName() const
{
	return getSearchTypeName(searchType);
}

SearchMatch::CommandType SearchMatch::getCommandType() const
{
	if (name == "overview")
	{
		return COMMAND_ALL;
	}
	else if (name == "error")
	{
		return COMMAND_ERROR;
	}

	return COMMAND_NODE_FILTER;
}
