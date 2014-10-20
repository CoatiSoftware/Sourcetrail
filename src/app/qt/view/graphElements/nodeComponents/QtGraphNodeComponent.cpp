#include "QtGraphNodeComponent.h"

QtGraphNodeComponent::QtGraphNodeComponent(const std::weak_ptr<QtGraphNode>& graphNode)
	: m_graphNode(graphNode)
{
}

QtGraphNodeComponent::~QtGraphNodeComponent()
{
}
