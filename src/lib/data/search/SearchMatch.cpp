#include "SearchMatch.h"

#include <sstream>

#include "NodeTypeSet.h"
#include "logging.h"

void SearchMatch::log(const std::vector<SearchMatch>& matches, const std::wstring& query)
{
	std::wstringstream ss;
	ss << std::endl << matches.size() << " matches for \"" << query << "\":" << std::endl;

	for (const SearchMatch& match: matches)
	{
		match.print(ss);
	}

	LOG_INFO(ss.str());
}

std::wstring SearchMatch::getSearchTypeName(SearchType type)
{
	switch (type)
	{
	case SEARCH_NONE:
		return L"none";
	case SEARCH_TOKEN:
		return L"token";
	case SEARCH_COMMAND:
		return L"command";
	case SEARCH_OPERATOR:
		return L"operator";
	case SEARCH_FULLTEXT:
		return L"fulltext";
	}

	return L"none";
}

std::wstring SearchMatch::searchMatchesToString(const std::vector<SearchMatch>& matches)
{
	std::wstringstream ss;

	for (const SearchMatch& match: matches)
	{
		ss << L'@' << match.getFullName() << L':'
		   << NodeType::getReadableTypeWString(match.nodeType.getType()) << L' ';
	}

	return ss.str();
}

SearchMatch SearchMatch::createCommand(CommandType type)
{
	SearchMatch match;
	match.name = getCommandName(type);
	match.text = match.name;
	match.typeName = L"command";
	match.searchType = SEARCH_COMMAND;
	return match;
}

std::vector<SearchMatch> SearchMatch::createCommandsForNodeTypes(NodeTypeSet types)
{
	std::vector<SearchMatch> matches;

	for (const NodeType& type: types.getNodeTypes())
	{
		SearchMatch match;
		match.name = type.getReadableTypeWString();
		match.text = match.name;
		match.typeName = L"filter";
		match.searchType = SEARCH_COMMAND;
		match.nodeType = type;
		matches.push_back(match);
	}

	return matches;
}

std::wstring SearchMatch::getCommandName(CommandType type)
{
	switch (type)
	{
	case COMMAND_ALL:
		return L"overview";
	case COMMAND_ERROR:
		return L"error";
	case COMMAND_NODE_FILTER:
		return L"node_filter";
	case COMMAND_LEGEND:
		return L"legend";
	}

	return L"none";
}

SearchMatch::SearchMatch()
	: typeName(L""), nodeType(NodeType::NODE_SYMBOL), searchType(SEARCH_NONE), hasChildren(false)
{
}

SearchMatch::SearchMatch(const std::wstring& query)
	: name(query)
	, text(query)
	, typeName(L"")
	, nodeType(NodeType::NODE_SYMBOL)
	, searchType(SEARCH_NONE)
	, hasChildren(false)
{
	tokenNames.emplace_back(query, NAME_DELIMITER_UNKNOWN);
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

	const std::wstring* str = &text;
	const std::wstring* otherStr = &other.text;
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
		if (towlower(str->at(i)) != towlower(otherStr->at(i)))
		{
			return towlower(str->at(i)) < towlower(otherStr->at(i));
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

	return getSearchTypeName() < other.getSearchTypeName();
}

bool SearchMatch::operator==(const SearchMatch& other) const
{
	return text == other.text && searchType == other.searchType;
}

size_t SearchMatch::getTextSizeForSorting(const std::wstring* str) const
{
	// check if templated symbol and only use size up to template stuff
	size_t pos = str->find(L'<');
	if (pos != std::wstring::npos)
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

void SearchMatch::print(std::wostream& ostream) const
{
	ostream << name << std::endl << L'\t';
	size_t i = 0;
	for (size_t index: indices)
	{
		while (i < index)
		{
			i++;
			ostream << L' ';
		}
		ostream << L'^';
		i++;
	}
	ostream << std::endl;
}

std::wstring SearchMatch::getFullName() const
{
	if (searchType == SEARCH_TOKEN && nodeType.isFile())
	{
		return text;
	}

	return name;
}

std::wstring SearchMatch::getSearchTypeName() const
{
	return getSearchTypeName(searchType);
}

SearchMatch::CommandType SearchMatch::getCommandType() const
{
	if (name == L"overview")
	{
		return COMMAND_ALL;
	}
	else if (name == L"error")
	{
		return COMMAND_ERROR;
	}
	else if (name == L"legend")
	{
		return COMMAND_LEGEND;
	}

	return COMMAND_NODE_FILTER;
}
