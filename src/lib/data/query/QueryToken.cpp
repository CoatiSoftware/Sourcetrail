#include "data/query/QueryToken.h"

QueryToken::QueryToken(const std::string& name)
	: m_name(name)
{
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

bool QueryToken::isComplete() const
{
	return true;
}

void QueryToken::print(std::ostream& ostream) const
{
	ostream << '"' << m_name << '"';
}

const std::string& QueryToken::getName() const
{
	return m_name;
}
