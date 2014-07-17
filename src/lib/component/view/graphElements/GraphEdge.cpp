#include "component/view/graphElements/GraphEdge.h"

GraphEdge::GraphEdge(const Id tokenId)
	: m_tokenId(tokenId)
{
}

GraphEdge::~GraphEdge()
{
}

Id GraphEdge::getTokenId() const
{
	return m_tokenId;
}
