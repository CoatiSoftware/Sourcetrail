#include "component/view/graphElements/GraphEdge.h"

GraphEdge::GraphEdge(const Edge* data)
	: m_data(data)
{
}

GraphEdge::~GraphEdge()
{
}

const Edge* GraphEdge::getData() const
{
	return m_data;
}
