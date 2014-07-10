#include "data/graph/edgeComponent/EdgeComponent.h"

EdgeComponent::EdgeComponent()
{
}

EdgeComponent::~EdgeComponent()
{
}

void EdgeComponent::setEdge(Edge* edge)
{
	m_edge = edge;
}

Edge* EdgeComponent::getEdge() const
{
	return m_edge;
}
