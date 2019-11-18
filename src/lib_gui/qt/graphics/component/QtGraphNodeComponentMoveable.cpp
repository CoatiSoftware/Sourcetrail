#include "QtGraphNodeComponentMoveable.h"

#include <QGraphicsSceneEvent>

#include "QtGraphNode.h"

QtGraphNodeComponentMoveable::QtGraphNodeComponentMoveable(QtGraphNode* graphNode)
	: QtGraphNodeComponent(graphNode), m_mouseOffset(0.0f, 0.0f)
{
}

QtGraphNodeComponentMoveable::~QtGraphNodeComponentMoveable() {}

void QtGraphNodeComponentMoveable::nodeMousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if (event->button() != Qt::LeftButton)
	{
		return;
	}

	m_oldPos = m_graphNode->getPosition();
	m_mouseOffset.x = event->scenePos().x() - m_oldPos.x;
	m_mouseOffset.y = event->scenePos().y() - m_oldPos.y;

	event->accept();
}

void QtGraphNodeComponentMoveable::nodeMouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	m_graphNode->setPosition(
		Vec2i(event->scenePos().x() - m_mouseOffset.x, event->scenePos().y() - m_mouseOffset.y));
	event->accept();
}

void QtGraphNodeComponentMoveable::nodeMouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if (event->button() != Qt::LeftButton)
	{
		return;
	}

	if (event->isAccepted())
	{
		return;
	}

	m_graphNode->moved(m_oldPos);
}
