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
	default:
		return "none";
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

SearchMatch::SearchMatch()
	: typeName("")
	, searchType(SEARCH_NONE)
{
}

SearchMatch::SearchMatch(const std::string& query)
	: nameHierarchy(query)
	, typeName("")
	, searchType(SEARCH_NONE)
{
}

bool SearchMatch::isValid() const
{
	return searchType != SEARCH_NONE;
}

void SearchMatch::print(std::ostream& ostream) const
{
	ostream << weight << '\t' << nameHierarchy.getFullName() << std::endl << '\t';
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
	return nameHierarchy.getFullName();
}

std::string SearchMatch::getNodeTypeAsString() const
{
	return Node::getTypeString(nodeType);
}

std::string SearchMatch::getSearchTypeName() const
{
	return getSearchTypeName(searchType);
}
