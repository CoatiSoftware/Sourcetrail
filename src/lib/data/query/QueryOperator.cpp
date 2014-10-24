#include "data/query/QueryOperator.h"

#include "data/query/QueryCommand.h"
#include "data/query/QueryToken.h"

const std::map<char, QueryOperator::OperatorType>& QueryOperator::getOperatorTypeMap()
{
	static std::map<char, OperatorType> operatorMap;

	if (operatorMap.size())
	{
		return operatorMap;
	}

	operatorMap.emplace(' ', OPERATOR_NONE);

	operatorMap.emplace('!', OPERATOR_NOT);
	operatorMap.emplace('.', OPERATOR_HAS);
	operatorMap.emplace('>', OPERATOR_SUB);
	operatorMap.emplace('&', OPERATOR_AND);
	operatorMap.emplace('|', OPERATOR_OR);

	operatorMap.emplace(QueryToken::BOUNDARY, OPERATOR_TOKEN);
	operatorMap.emplace(QueryCommand::BOUNDARY, OPERATOR_COMMAND);

	operatorMap.emplace('(', OPERATOR_GROUP_OPEN);
	operatorMap.emplace(')', OPERATOR_GROUP_CLOSE);

	return operatorMap;
}

QueryOperator::OperatorType QueryOperator::getOperatorType(char c)
{
	const std::map<char, OperatorType>& operatorMap = getOperatorTypeMap();
	std::map<char, OperatorType>::const_iterator it = operatorMap.find(c);

	if (it != operatorMap.end())
	{
		return it->second;
	}

	return OPERATOR_NONE;
}

char QueryOperator::getOperator(OperatorType t)
{
	for (const std::pair<char, OperatorType>& p : getOperatorTypeMap())
	{
		if (p.second == t)
		{
			return p.first;
		}
	}

	return '\0';
}

QueryOperator::QueryOperator(OperatorType type, bool isImplicit)
	: m_type(type)
	, m_isImplicit(isImplicit)
{
}

QueryOperator::~QueryOperator()
{
}

bool QueryOperator::isCommand() const
{
	return false;
}

bool QueryOperator::isOperator() const
{
	return true;
}

bool QueryOperator::isToken() const
{
	return false;
}

bool QueryOperator::derivedIsComplete() const
{
	if (m_type == OPERATOR_NOT)
	{
		return bool(getRight());
	}

	return getLeft() && getRight();
}

std::string QueryOperator::getName() const
{
	return std::string(1, getOperator(m_type));
}

void QueryOperator::print(std::ostream& ostream) const
{
	ostream << getOperator(m_type);

	if (isImplicit())
	{
		ostream << " IMPLICIT";
	}
}

void QueryOperator::print(std::ostream& ostream, int n) const
{
	if (m_left)
	{
		m_left->print(ostream, n + 1);
	}

	QueryNode::print(ostream, n);

	if (m_right)
	{
		m_right->print(ostream, n + 1);
	}
}

std::shared_ptr<QueryNode> QueryOperator::getLeft() const
{
	return m_left;
}

void QueryOperator::setLeft(std::shared_ptr<QueryNode> node)
{
	m_left = node;
}

std::shared_ptr<QueryNode> QueryOperator::getRight() const
{
	return m_right;
}

void QueryOperator::setRight(std::shared_ptr<QueryNode> node)
{
	m_right = node;
}

QueryOperator::OperatorType QueryOperator::getType() const
{
	return m_type;
}

bool QueryOperator::lowerPrecedence(const QueryOperator& other)
{
	return m_type < other.m_type;
}

bool QueryOperator::isImplicit() const
{
	return m_isImplicit;
}
