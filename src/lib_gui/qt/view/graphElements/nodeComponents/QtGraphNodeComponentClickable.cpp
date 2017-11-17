#include "QtGraphNodeComponentClickable.h"

#include <QGraphicsSceneEvent>

#include "qt/view/graphElements/QtGraphNode.h"

QtGraphNodeComponentClickable::QtGraphNodeComponentClickable(QtGraphNode* graphNode)
	: QtGraphNodeComponent(graphNode)
	, m_mousePos(0.0f, 0.0f)
	, m_mouseMoved(false)
{
}

QtGraphNodeComponentClickable::~QtGraphNodeComponentClickable()
{
}

void QtGraphNodeComponentClickable::nodeMousePressEvent(QGraphicsSceneMouseEvent* event)
{
	m_mousePos = Vec2i(event->scenePos().x(), event->scenePos().y());
	m_mouseMoved = false;
}

void QtGraphNodeComponentClickable::nodeMouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	Vec2i mousePos = Vec2i(event->scenePos().x(), event->scenePos().y());

	if ((mousePos - m_mousePos).getLength() > 1.0f)
	{
		m_mouseMoved = true;
	}
}

void QtGraphNodeComponentClickable::nodeMouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if (!m_mouseMoved)
	{
		m_graphNode->onClick();
		event->accept();
	}
}
