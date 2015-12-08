#include "QtGraphNodeComponentMoveable.h"

#include <QGraphicsSceneEvent>

#include "qt/view/graphElements/QtGraphNode.h"

QtGraphNodeComponentMoveable::QtGraphNodeComponentMoveable(const std::weak_ptr<QtGraphNode>& graphNode)
	: QtGraphNodeComponent(graphNode)
	, m_mouseOffset(0.0f, 0.0f)
{
}

QtGraphNodeComponentMoveable::~QtGraphNodeComponentMoveable()
{
}

void QtGraphNodeComponentMoveable::nodeMousePressEvent(QGraphicsSceneMouseEvent* event)
{
	std::shared_ptr<QtGraphNode> node = m_graphNode.lock();
	if (node != NULL)
	{
		m_oldPos = node->getPosition();
		m_mouseOffset.x = event->scenePos().x() - m_oldPos.x;
		m_mouseOffset.y = event->scenePos().y() - m_oldPos.y;

		event->accept();
	}
}

void QtGraphNodeComponentMoveable::nodeMouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	std::shared_ptr<QtGraphNode> node = m_graphNode.lock();
	if (node != NULL)
	{
		node->setPosition(Vec2i(event->scenePos().x() - m_mouseOffset.x, event->scenePos().y() - m_mouseOffset.y));
		event->accept();
	}
}

void QtGraphNodeComponentMoveable::nodeMouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if (event->isAccepted())
	{
		return;
	}

	std::shared_ptr<QtGraphNode> node = m_graphNode.lock();
	if (node != NULL)
	{
		node->moved(m_oldPos);
	}
}
