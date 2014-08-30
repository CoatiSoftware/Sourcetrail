#include "data/query/QueryNode.h"

QueryNode::QueryNode()
	: m_isGroup(false)
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
