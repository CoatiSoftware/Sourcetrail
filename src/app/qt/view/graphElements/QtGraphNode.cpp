#include "qt/view/graphElements/QtGraphNode.h"

#include <sstream>

#include <QGraphicsScene>
#include <QGraphicsSceneEvent>

#include "qt/view/graphElements/QtGraphEdge.h"
#include "qt/view/graphElements/nodeComponents/QtGraphNodeComponent.h"

QtGraphNode::QtGraphNode(const Vec2i& position, const Vec2i& size, const std::string& name, const Id tokenId)
	: GraphNode(tokenId)
	, m_baseSize(size)
	, m_currentSize(size)
	, m_padding(10, 10)
	, m_contentHidden(false)
{
	this->setRect(0, 0, size.x, size.y);
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

Vec2i QtGraphNode::getSize() const
{
	return m_currentSize;
}

void QtGraphNode::setParent(std::weak_ptr<QtGraphNode> parentNode)
{
	m_parentNode = parentNode;

	std::shared_ptr<QtGraphNode> parent = parentNode.lock();
	if(parent != NULL)
	{
		QGraphicsRectItem::setParentItem(parent.get());
	}
}

void QtGraphNode::addComponent(const std::shared_ptr<QtGraphNodeComponent>& component)
{
	m_components.push_back(component);
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

	rebuildLayout();
}

void QtGraphNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	for (std::list<std::shared_ptr<QtGraphNodeComponent>>::iterator it = m_components.begin(); it != m_components.end(); it++)
	{
		(*it)->nodeMousePressEvent(event);
	}
}

void QtGraphNode::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	for (std::list<std::shared_ptr<QtGraphNodeComponent>>::iterator it = m_components.begin(); it != m_components.end(); it++)
	{
		(*it)->nodeMouseMoveEvent(event);
	}
}

void QtGraphNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	MessageActivateToken message(m_tokenId);
	message.dispatch();

	for (std::list<std::shared_ptr<QtGraphNodeComponent>>::iterator it = m_components.begin(); it != m_components.end(); it++)
	{
		(*it)->nodeMouseDoubleClickEvent(event);
	}
}

void QtGraphNode::notifyParentMoved()
{
	notifyEdgesAfterMove();
}

void QtGraphNode::hideContent()
{
	for (std::list<std::shared_ptr<GraphNode>>::iterator it = m_subNodes.begin(); it != m_subNodes.end(); it++)
	{
		(*it)->hideContent();

		if((*it)->getEdgeCountRecursive() <= 0)
		{
			(*it)->hide();
		}
	}

	m_contentHidden = true;

	rebuildLayout();

	notifyParentNodeAfterSizeChanged();
}

void QtGraphNode::showContent()
{
	for (std::list<std::shared_ptr<GraphNode>>::iterator it = m_subNodes.begin(); it != m_subNodes.end(); it++)
	{
		(*it)->show();
	}

	m_contentHidden = false;

	rebuildLayout();

	notifyParentNodeAfterSizeChanged();
}

void QtGraphNode::hide()
{
	QGraphicsRectItem::hide();
}

void QtGraphNode::show()
{
	QGraphicsRectItem::show();
}

bool QtGraphNode::isHidden()
{
	return !QGraphicsRectItem::isVisible();
}

bool QtGraphNode::contentIsHidden()
{
	return m_contentHidden;
}

unsigned int QtGraphNode::getOutEdgeCount() const
{
	return m_outEdges.size();
}

unsigned int QtGraphNode::getInEdgeCount() const
{
	return m_inEdges.size();
}

unsigned int QtGraphNode::getEdgeCountRecursive() const
{
	unsigned int result = 0;

	for (std::list<std::shared_ptr<GraphNode>>::const_iterator it = m_subNodes.begin(); it != m_subNodes.end(); it++)
	{
		result += (*it)->getEdgeCountRecursive();
	}

	result += (getInEdgeCount() + getOutEdgeCount());

	return result;
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

void QtGraphNode::notifyParentNodeAfterSizeChanged()
{
	std::shared_ptr<QtGraphNode> parentNode = m_parentNode.lock();

	if(parentNode != NULL)
	{
		parentNode->onChildSizeChanged();
	}
}

void QtGraphNode::onChildSizeChanged()
{
	rebuildLayout();

	notifyParentNodeAfterSizeChanged();
}

void QtGraphNode::rebuildLayout()
{
	int nextYPos = m_baseSize.y;
	int newWidth = m_baseSize.x;

	for (std::list<std::shared_ptr<GraphNode>>::iterator it = m_subNodes.begin(); it != m_subNodes.end(); it++)
	{
		if((*it)->isHidden() == false)
		{
			(*it)->setPosition(this->getPosition() + Vec2i(m_padding.x, nextYPos));
			nextYPos += (*it)->getSize().y + m_padding.y;

			int tmpWidth = ((*it)->getPosition().x - getPosition().x) + (*it)->getSize().x + m_padding.x;

			if(tmpWidth > newWidth)
			{
				newWidth = tmpWidth;
			}
		}
	}

	m_currentSize.x = newWidth;
	m_currentSize.y = nextYPos;

	this->setRect(0, 0, m_currentSize.x, m_currentSize.y);
}
