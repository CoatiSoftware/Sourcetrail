#include "qt/view/graphElements/QtGraphNode.h"

#include "qgraphicsscene.h"
#include "qgraphicssceneevent.h"

#include "qt/view/graphElements/QtGraphEdge.h"

QtGraphNode::QtGraphNode(const Vec2i& position, const std::string& name, const Id tokenId)
	: GraphNode(tokenId)
{
	this->setRect(0, 0, 100, 100);
	this->setPos(position.x, position.y);
	QBrush brush(Qt::lightGray);
	this->setBrush(brush);

	m_text = new QGraphicsTextItem(this);
	m_text->setPos(0, 0);
	m_text->setPlainText(QString(name.c_str()));
}

QtGraphNode::~QtGraphNode()
{
}

std::string QtGraphNode::getName()
{
	return m_text->toPlainText().toStdString();
}

Vec2i QtGraphNode::getPosition()
{
	return Vec2i(this->scenePos().x(), this->scenePos().y());
}

void QtGraphNode::addOutEdge(const std::shared_ptr<GraphEdge>& edge)
{
	m_outEdges.push_back(edge);
}

void QtGraphNode::addInEdge(const std::weak_ptr<GraphEdge>& edge)
{
	m_inEdges.push_back(edge);
}

void QtGraphNode::removeOutEdge(const std::shared_ptr<GraphEdge>& edge)
{
	std::list<std::shared_ptr<GraphEdge> >::iterator it = m_outEdges.begin();
	for(it; it != m_outEdges.end(); it++)
	{
		if(*it == edge)
		{
			m_outEdges.erase(it, it);
			break;
		}
	}
}

void QtGraphNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	m_mouseOffset.x = event->pos().x();
	m_mouseOffset.y = event->pos().y();

	MessageActivateToken message(m_tokenId);
	message.dispatch();
}

void QtGraphNode::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	this->setPos(event->scenePos().x() - m_mouseOffset.x, event->scenePos().y() - m_mouseOffset.y);

	Vec2i p(event->scenePos().x(), event->scenePos().y());

	std::list<std::shared_ptr<GraphEdge> >::iterator it = m_outEdges.begin();
	for(it; it != m_outEdges.end(); it++)
	{
		(*it)->ownerMoved();
	}

	std::list<std::weak_ptr<GraphEdge> >::iterator it2 = m_inEdges.begin();
	for(it2; it2 != m_inEdges.end(); it2++)
	{
		 std::shared_ptr<GraphEdge> edge = it2->lock();
		 if(edge.get() != NULL)
		 {
			 edge->targetMoved();
		 }
		 else
		 {
			 m_inEdges.erase(it2, it2);
		 }
	}
}
