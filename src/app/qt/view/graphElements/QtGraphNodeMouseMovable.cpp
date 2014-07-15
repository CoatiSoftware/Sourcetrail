#include "qt/view/graphElements/QtGraphNodeMouseMovable.h"

#include "qgraphicssceneevent.h"

#include "qt/view/graphElements/QtGraphEdge.h"

QtGraphNodeMouseMovable::QtGraphNodeMouseMovable(const Vec2i& position, const std::string& name, const Id tokenId)
	: QtGraphNode(position, name, tokenId)
{
}

QtGraphNodeMouseMovable::~QtGraphNodeMouseMovable()
{
}

void QtGraphNodeMouseMovable::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	m_mouseOffset.x = event->pos().x();
	m_mouseOffset.y = event->pos().y();
}

void QtGraphNodeMouseMovable::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	this->setPos(event->scenePos().x() - m_mouseOffset.x, event->scenePos().y() - m_mouseOffset.y);

	Vec2i p(event->scenePos().x(), event->scenePos().y());

	notifyEdgesAfterMove();
}