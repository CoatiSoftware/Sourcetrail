#include "qt/view/graphElements/QtGraphNodeMovable.h"

#include "qgraphicssceneevent.h"

#include "qt/view/graphElements/QtGraphEdge.h"

QtGraphNodeMovable::QtGraphNodeMovable(const Vec2i& position, const std::string& name, const Id tokenId)
	: QtGraphNode(position, name, tokenId)
{
}

QtGraphNodeMovable::~QtGraphNodeMovable()
{
}

void QtGraphNodeMovable::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	m_mouseOffset.x = event->pos().x();
	m_mouseOffset.y = event->pos().y();
}

void QtGraphNodeMovable::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	this->setPos(event->scenePos().x() - m_mouseOffset.x, event->scenePos().y() - m_mouseOffset.y);

	Vec2i p(event->scenePos().x(), event->scenePos().y());

	std::list<std::shared_ptr<GraphEdge> >::iterator it = m_outEdges.begin();
	for(it; it != m_outEdges.end(); it++)
	{
		(*it)->ownerMoved();
	}

	std::list<std::weak_ptr<GraphEdge> >::iterator it2 = m_inEdges.begin();
	while(it2 != m_inEdges.end())
	{
		 std::shared_ptr<GraphEdge> edge = it2->lock();
		 if(edge.get() != NULL)
		 {
			 edge->targetMoved();
			 ++it2;
		 }
		 else
		 {
			 m_inEdges.erase(it2++);
		 }
	}
}