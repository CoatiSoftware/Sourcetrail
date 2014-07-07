#include "component/view/graphElements/GraphNode.h"

GraphNode::GraphNode(const Id tokenId)
	: m_tokenId(tokenId)
{
}

GraphNode::~GraphNode()
{
}

Id GraphNode::getTokenId()
{
	return m_tokenId;
}
