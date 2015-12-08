#include "QtGraphNodeComponent.h"

QtGraphNodeComponent::QtGraphNodeComponent(const std::weak_ptr<QtGraphNode>& graphNode)
	: m_graphNode(graphNode)
{
}

QtGraphNodeComponent::~QtGraphNodeComponent()
{
}

void QtGraphNodeComponent::nodeMousePressEvent(QGraphicsSceneMouseEvent* event)
{
}

void QtGraphNodeComponent::nodeMouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
}

void QtGraphNodeComponent::nodeMouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
}
