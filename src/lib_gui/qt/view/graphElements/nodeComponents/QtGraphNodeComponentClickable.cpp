#include "QtGraphNodeComponentClickable.h"

#include <QGraphicsSceneEvent>

#include "QtGraphNode.h"

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
	if (event->button() != Qt::LeftButton && event->button() != Qt::MiddleButton)
	{
		return;
	}

	m_mousePos = Vec2i(event->scenePos().x(), event->scenePos().y());
	m_mouseMoved = false;

	if (event->button() == Qt::MiddleButton)
	{
		event->accept();
	}
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
	if (event->button() != Qt::LeftButton && event->button() != Qt::MiddleButton)
	{
		return;
	}

	if (!m_mouseMoved)
	{
		if (event->modifiers() & Qt::AltModifier && event->button() == Qt::LeftButton)
		{
			m_graphNode->onHide();
		}
		else if (event->modifiers() & Qt::ShiftModifier && event->button() == Qt::LeftButton)
		{
			m_graphNode->onCollapseExpand();
		}
		else if (event->modifiers() & Qt::ControlModifier && event->button() == Qt::LeftButton)
		{
			m_graphNode->onShowDefinition();
		}
		else
		{
			if (event->button() == Qt::MiddleButton)
			{
				m_graphNode->onMiddleClick();
			}
			else
			{
				m_graphNode->onClick();
			}
		}
		event->accept();
	}
}
