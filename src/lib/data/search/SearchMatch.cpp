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

std::string SearchMatch::getCommandName(CommandType type)
{
	switch (type)
	{
	case COMMAND_ALL:
		return "overview";
	case COMMAND_ERROR:
		return "error";
	case COMMAND_COLOR_SCHEME_TEST:
		return "color_scheme_test";
	}

	return "none";
}

SearchMatch::CommandType SearchMatch::getCommandType(const std::string& name)
{
	if (name == "overview")
	{
		return COMMAND_ALL;
	}
	else if (name == "error")
	{
		return COMMAND_ERROR;
	}
	else if (name == "color_scheme_test")
	{
		return COMMAND_COLOR_SCHEME_TEST;
	}

	return COMMAND_ALL;
}

SearchMatch::SearchMatch()
	: typeName("")
	, searchType(SEARCH_NONE)
	, hasChildren(false)
{
}

SearchMatch::SearchMatch(const std::string& query)
	: name(query)
	, typeName("")
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

	// text size
	if (str->size() < otherStr->size())
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

bool SearchMatch::isValid() const
{
	return searchType != SEARCH_NONE;
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
	if (searchType == SEARCH_TOKEN && nodeType == Node::NODE_FILE)
	{
		return text;
	}

	return name;
}

std::string SearchMatch::getNodeTypeAsString() const
{
	return Node::getTypeString(nodeType);
}

std::string SearchMatch::getSearchTypeName() const
{
	return getSearchTypeName(searchType);
}
