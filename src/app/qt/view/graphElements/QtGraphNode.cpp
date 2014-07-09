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
	std::list<std::weak_ptr<GraphEdge> >::iterator it = m_inEdges.begin();
	for(it; it != m_inEdges.end(); it++)
	{
		std::shared_ptr<GraphEdge> edge = it->lock();
		if(edge != NULL)
		{
			edge->removeEdgeFromScene();
		}
	}
}

std::string QtGraphNode::getName()
{
	return m_text->toPlainText().toStdString();
}

Vec2i QtGraphNode::getPosition()
{
	return Vec2i(this->scenePos().x(), this->scenePos().y());
}

bool QtGraphNode::addOutEdge(const std::shared_ptr<GraphEdge>& edge)
{
	std::list<std::shared_ptr<GraphEdge> >::iterator it = m_outEdges.begin();
	for(it; it != m_outEdges.end(); it++)
	{
		if ((*it)->getOwner().lock() == edge->getOwner().lock() && (*it)->getTarget().lock() == edge->getTarget().lock())
		{
			return false;
		}
	}

	m_outEdges.push_back(edge);
	return true;
}

bool QtGraphNode::addInEdge(const std::weak_ptr<GraphEdge>& edge)
{
	std::list<std::weak_ptr<GraphEdge> >::iterator it = m_inEdges.begin();
	for(it; it != m_inEdges.end(); it++)
	{
		std::shared_ptr<GraphEdge> existingEdge = it->lock();
		if(existingEdge != NULL)
		{
			if (existingEdge->getOwner().lock() == edge.lock()->getOwner().lock() &&
				existingEdge->getTarget().lock() == edge.lock()->getTarget().lock())
			{
				return false;
			}
		}
	}

	m_inEdges.push_back(edge);
	return true;
}

void QtGraphNode::removeOutEdge(GraphEdge* edge)
{
	std::list<std::shared_ptr<GraphEdge> >::iterator it = m_outEdges.begin();
	while(it != m_outEdges.end())
	{
		if((*it).get() == edge)
		{
			m_outEdges.erase(it);
			break;
		}

		++it;
	}
}

void QtGraphNode::addSubNode(const std::shared_ptr<GraphNode>& node)
{
	m_subNodes.push_back(node);
}

void QtGraphNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	MessageActivateToken message(m_tokenId);
	message.dispatch();
}
