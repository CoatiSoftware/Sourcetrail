#include "component/view/graphElements/GraphEdge.h"

GraphEdge::GraphEdge(const Edge* data)
	: m_data(data)
{
}

GraphEdge::~GraphEdge()
{
}

Id GraphEdge::getTokenId() const
{
	return m_data->getId();
}

const Edge* GraphEdge::getData() const
{
	return m_data;
}
