#include "qt/view/graphElements/QtGraphNode.h"

#include <sstream>

#include <QGraphicsScene>
#include <QGraphicsSceneEvent>

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
	std::stringstream text;
	text << m_tokenId << " -> " << name;
	m_text->setPlainText(QString(text.str().c_str()));
}

QtGraphNode::~QtGraphNode()
{
	for (std::list<std::weak_ptr<GraphEdge>>::iterator it = m_inEdges.begin(); it != m_inEdges.end(); it++)
	{
		std::shared_ptr<GraphEdge> edge = it->lock();
		if (edge != NULL)
		{
			edge->removeEdgeFromScene();
		}
	}
}

std::string QtGraphNode::getName() const
{
	return m_text->toPlainText().toStdString();
}

Vec2i QtGraphNode::getPosition() const
{
	return Vec2i(this->scenePos().x(), this->scenePos().y());
}

void QtGraphNode::setPosition(const Vec2i& position)
{
	Vec2i currentPosition = getPosition();
	Vec2i offset = position - currentPosition;

	if (offset.getLength() > 0.0f)
	{
		this->moveBy(offset.x, offset.y);
		notifyEdgesAfterMove();
	}
}

bool QtGraphNode::addOutEdge(const std::shared_ptr<GraphEdge>& edge)
{
	for (std::list<std::shared_ptr<GraphEdge>>::iterator it = m_outEdges.begin(); it != m_outEdges.end(); it++)
	{
		if ((*it)->getOwner().lock() == edge->getOwner().lock() &&
			(*it)->getTarget().lock() == edge->getTarget().lock())
		{
			return false;
		}
	}

	m_outEdges.push_back(edge);
	return true;
}

bool QtGraphNode::addInEdge(const std::weak_ptr<GraphEdge>& edge)
{
	for (std::list<std::weak_ptr<GraphEdge>>::iterator it = m_inEdges.begin(); it != m_inEdges.end(); it++)
	{
		std::shared_ptr<GraphEdge> existingEdge = it->lock();
		if (existingEdge != NULL)
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
	std::list<std::shared_ptr<GraphEdge>>::iterator it = m_outEdges.begin();
	while (it != m_outEdges.end())
	{
		if ((*it).get() == edge)
		{
			m_outEdges.erase(it);
			break;
		}

		++it;
	}
}

std::list<std::shared_ptr<GraphNode>> QtGraphNode::getSubNodes() const
{
	return m_subNodes;
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

void QtGraphNode::notifyParentMoved()
{
	notifyEdgesAfterMove();
}

void QtGraphNode::notifyEdgesAfterMove()
{
	for (std::list<std::shared_ptr<GraphEdge>>::iterator it = m_outEdges.begin(); it != m_outEdges.end(); it++)
	{
		(*it)->ownerMoved();
	}

	std::list<std::weak_ptr<GraphEdge>>::iterator it2 = m_inEdges.begin();
	while (it2 != m_inEdges.end())
	{
		 std::shared_ptr<GraphEdge> edge = it2->lock();
		 if (edge.get() != NULL)
		 {
			 edge->targetMoved();
			 ++it2;
		 }
		 else
		 {
			 m_inEdges.erase(it2++);
		 }
	}

	for (std::list<std::shared_ptr<GraphNode>>::iterator it3 = m_subNodes.begin(); it3 != m_subNodes.end(); it3++)
	{
		(*it3)->notifyParentMoved();
	}
}
