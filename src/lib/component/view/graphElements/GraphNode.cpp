#include "component/view/graphElements/GraphNode.h"

GraphNode::GraphNode(const Node* data)
	: m_data(data)
{
}

GraphNode::~GraphNode()
{
}

Id GraphNode::getTokenId() const
{
	if (m_data)
	{
		return m_data->getId();
	}
	return 0;
}

const Node* GraphNode::getData() const
{
	return m_data;
}

void GraphNode::setData(const Node* data)
{
	m_data = data;
}
