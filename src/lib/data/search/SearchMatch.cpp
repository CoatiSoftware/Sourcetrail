#include "data/search/SearchMatch.h"

#include <sstream>

#include "utility/logging/logging.h"

#include "data/query/QueryCommand.h"
#include "data/query/QueryToken.h"

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

void SearchMatch::print(std::ostream& ostream) const
{
	ostream << weight << '\t' << fullName << std::endl << '\t';
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

std::string SearchMatch::encodeForQuery() const
{
	if (!tokenIds.size())
	{
		return QueryCommand::BOUNDARY + fullName + QueryCommand::BOUNDARY;
	}

	std::stringstream ss;
	ss << QueryToken::BOUNDARY << fullName;
	for (Id tokenId : tokenIds)
	{
		ss << QueryToken::DELIMITER << tokenId;
	}
	ss << QueryToken::BOUNDARY;
	return ss.str();
}
