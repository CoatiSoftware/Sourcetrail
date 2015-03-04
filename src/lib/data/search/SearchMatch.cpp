#include "data/search/SearchMatch.h"

#include <sstream>
#include <cstdlib>

#include "data/query/QueryCommand.h"
#include "data/query/QueryOperator.h"
#include "data/query/QueryToken.h"
#include "utility/logging/logging.h"
#include "utility/utilityString.h"

SearchMatch::SearchMatch()
	: fullName("")
	, typeName("")
	, queryNodeType(QueryNode::QUERYNODETYPE_NONE)
{
}

SearchMatch::SearchMatch(const std::string& query)
{
	decodeFromQuery(query);
}

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
	switch(queryNodeType)
	{
	case QueryNode::QUERYNODETYPE_COMMAND:
		return QueryCommand::BOUNDARY + fullName + QueryCommand::BOUNDARY;
	case QueryNode::QUERYNODETYPE_TOKEN:
	{
		std::stringstream ss;
		ss << QueryToken::BOUNDARY << fullName;
		ss << QueryToken::DELIMITER << nodeType;
		for (Id tokenId : tokenIds)
		{
			ss << QueryToken::DELIMITER << tokenId;
		}
		ss << QueryToken::BOUNDARY;
		return ss.str();
	}

	case QueryNode::QUERYNODETYPE_OPERATOR:
	case QueryNode::QUERYNODETYPE_NONE:
		return fullName;
	}
}

void SearchMatch::decodeFromQuery(std::string query)
{
	if (query.size() > 2 && query.front() == QueryToken::BOUNDARY && query.back() == QueryToken::BOUNDARY)
	{
		queryNodeType = QueryNode::QUERYNODETYPE_TOKEN;
		query.erase(query.begin());
		query.pop_back();
	}
	else if (query.size() > 2 && query.front() == QueryCommand::BOUNDARY && query.back() == QueryCommand::BOUNDARY)
	{
		queryNodeType = QueryNode::QUERYNODETYPE_COMMAND;
		query.erase(query.begin());
		query.pop_back();
	}
	else if (query.size() == 1 && QueryOperator::getOperatorType(query.front()) != QueryOperator::OPERATOR_NONE)
	{
		queryNodeType = QueryNode::QUERYNODETYPE_OPERATOR;
	}
	else
	{
		queryNodeType = QueryNode::QUERYNODETYPE_NONE;
	}

	std::vector<std::string> queryParts = utility::splitToVector(query, QueryToken::DELIMITER);

	fullName = queryParts[0];

	if(queryParts.size() > 1)
	{
		for(int i = 2; i < queryParts.size(); ++i)
		{
			tokenIds.insert(std::strtoul(queryParts[i].c_str(),nullptr,0));
		}
	}

}


std::deque<SearchMatch> SearchMatch::stringDequeToSearchMatchDeque(const std::deque<std::string>& stringDeque)
{
	std::deque<SearchMatch> matchDeque;
	for(std::string str : stringDeque)
	{
		matchDeque.push_back(SearchMatch(str));
	}
	return matchDeque;
}

std::deque<std::string> SearchMatch::searchMatchDequeToStringDeque(const std::deque<SearchMatch>& searchMatchDeque)
{
	std::deque<std::string> stringDeque;
	for(SearchMatch match : searchMatchDeque)
	{
		stringDeque.push_back(match.encodeForQuery());
	}
	return stringDeque;

}

std::string SearchMatch::getNodeTypeAsString() const
{
	return Node::getTypeString(nodeType);
}
