#include "data/query/QueryNode.h"

QueryNode::QueryNode(QueryNodeType queryNodeType)
	: m_nodeType(queryNodeType)
	, m_isGroup(false)
	, m_isComplete(true)
{
}

QueryNode::~QueryNode()
{
}

void QueryNode::print(std::ostream& ostream, int n) const
{
	for (int i = 0; i < n; i++)
	{
		ostream << '\t';
	}

	if (isGroup())
	{
		ostream << '(';
	}

	print(ostream);

	if (isGroup())
	{
		ostream << ')';
	}

	if (!isComplete())
	{
		ostream << " INVALID";
	}

	ostream << '\n';
}

bool QueryNode::isGroup() const
{
	return m_isGroup;
}

void QueryNode::setIsGroup(bool isGroup)
{
	m_isGroup = isGroup;
}

bool QueryNode::isComplete() const
{
	return m_isComplete && derivedIsComplete();
}

void QueryNode::setIsComplete(bool isComplete)
{
	m_isComplete = isComplete;
}

QueryNode::QueryNodeType QueryNode::getQueryNodeType() const
{
	return m_nodeType;
}

std::string QueryNode::queryNodeTypeToString(const QueryNodeType& type)
{
	switch(type)
	{
		case QUERYNODETYPE_NONE:
			return "none";
		case QUERYNODETYPE_COMMAND:
			return "command";
		case QUERYNODETYPE_TOKEN:
			return "token";
		case QUERYNODETYPE_OPERATOR:
			return "operator";
	}
}

std::string QueryNode::getQueryNodeTypeAsString() const
{
	return queryNodeTypeToString(m_nodeType);
}