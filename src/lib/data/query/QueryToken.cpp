#include "data/query/QueryToken.h"

#include <deque>

#include "utility/utilityString.h"

QueryToken::QueryToken(std::string name)
{
	name.erase(std::remove(name.begin(), name.end(), BOUNDARY), name.end());
	std::deque<std::string> names = utility::split(name, DELIMITER);

	m_tokenName = names.front();
	names.pop_front();

	while (names.size())
	{
		std::stringstream ss;
		ss << names.front();
		names.pop_front();

		Id tokenId = 0;
		ss >> tokenId;
		if (tokenId)
		{
			m_tokenIds.insert(tokenId);
		}
	}
}

QueryToken::~QueryToken()
{
}

bool QueryToken::isCommand() const
{
	return false;
}

bool QueryToken::isOperator() const
{
	return false;
}

bool QueryToken::isToken() const
{
	return true;
}

bool QueryToken::derivedIsComplete() const
{
	return true;
}

std::string QueryToken::getName() const
{
	return m_tokenName;
}

void QueryToken::print(std::ostream& ostream) const
{
	ostream << BOUNDARY << m_tokenName;
	for (Id tokenId : m_tokenIds)
	{
		ostream << DELIMITER << tokenId;
	}
	ostream << BOUNDARY;
}

const std::string& QueryToken::getTokenName() const
{
	return m_tokenName;
}

const std::set<Id>& QueryToken::getTokenIds() const
{
	return m_tokenIds;
}

const char QueryToken::DELIMITER = ',';
const char QueryToken::BOUNDARY = '"';
