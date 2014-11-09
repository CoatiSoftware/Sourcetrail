#include "qt/view/graphElements/QtGraphNode.h"

#include <sstream>

#include <QFontMetrics>
#include <QGraphicsScene>
#include <QGraphicsSceneEvent>

#include "qt/view/graphElements/QtGraphEdge.h"
#include "qt/view/graphElements/nodeComponents/QtGraphNodeComponent.h"

QtGraphNode::QtGraphNode(const Node* data, TokenComponentAccess::AccessType accessType)
	: GraphNode(data)
	, m_isActive(false)
	, m_padding(10, 10)
	, m_contentHidden(false)
{
	m_text = new QGraphicsTextItem(this);
	m_text->setPos(0, 0);

	std::stringstream text;
	QBrush brush(Qt::white);

	if (data)
	{
		text << data->getId() << ": " << data->getName();
		brush.setColor(Qt::lightGray);
	}
	else
	{
		text << TokenComponentAccess::getAccessString(accessType);
	}

	m_text->setPlainText(QString(text.str().c_str()));
	this->setBrush(brush);

	m_baseSize.x = QFontMetrics(m_text->font()).width(m_text->toPlainText()) + 10;
	m_baseSize.y = 25;

	m_currentSize = m_baseSize;

	this->setRect(0, 0, m_baseSize.x, m_baseSize.y);
	this->setAcceptHoverEvents(true);
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

bool QtGraphNode::getIsActive() const
{
	return m_isActive;
}

void QtGraphNode::setIsActive(bool isActive)
{
	m_isActive = isActive;

	QPen p = this->pen();
	if (isActive)
	{
		p.setWidth(2);
	}
	else
	{
		p.setWidth(1);
	}
	this->setPen(p);
}

QtGraphNode* QtGraphNode::getParent() const
{
	return m_parentNode.lock().get();
}

void QtGraphNode::setParent(std::weak_ptr<QtGraphNode> parentNode)
{
	m_parentNode = parentNode;

	std::shared_ptr<QtGraphNode> parent = parentNode.lock();
	if (parent != NULL)
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

void QtGraphNode::notifyParentMoved()
{
	notifyEdgesAfterMove();
}

void QtGraphNode::hideContent()
{
	for (std::shared_ptr<GraphNode> node : m_subNodes)
	{
		node->hideContent();

		if (node->getTokenId() && node->getEdgeAndActiveCountRecursive() <= 0)
		{
			node->hide();
		}
	}

	m_contentHidden = true;

	onChildSizeChanged();
}

void QtGraphNode::showContent()
{
	for (std::shared_ptr<GraphNode> node : m_subNodes)
	{
		node->show();
	}

	m_contentHidden = false;

	onChildSizeChanged();
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

unsigned int QtGraphNode::getEdgeAndActiveCountRecursive() const
{
	unsigned int result = 0;

	if (m_isActive)
	{
		result++;
	}

	for (std::shared_ptr<GraphNode> node : m_subNodes)
	{
		result += node->getEdgeAndActiveCountRecursive();
	}

	result += (getInEdgeCount() + getOutEdgeCount());

	return result;
}

void QtGraphNode::onClick()
{
	if (m_data)
	{
		MessageActivateToken(m_data->getId()).dispatch();
	}
}

void QtGraphNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	event->ignore();

	for (std::shared_ptr<QtGraphNodeComponent> component : m_components)
	{
		component->nodeMousePressEvent(event);
	}

	if (!event->isAccepted())
	{
		QtGraphNode* parent = getParent();
		if (parent)
		{
			parent->mousePressEvent(event);
		}
	}
}

void QtGraphNode::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	event->ignore();

	for (std::shared_ptr<QtGraphNodeComponent> component : m_components)
	{
		component->nodeMouseMoveEvent(event);
	}

	if (!event->isAccepted())
	{
		QtGraphNode* parent = getParent();
		if (parent)
		{
			parent->mouseMoveEvent(event);
		}
	}
}

void QtGraphNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	event->ignore();

	for (std::shared_ptr<QtGraphNodeComponent> component : m_components)
	{
		component->nodeMouseReleaseEvent(event);
	}

	if (!event->isAccepted())
	{
		QtGraphNode* parent = getParent();
		if (parent)
		{
			parent->mouseReleaseEvent(event);
		}
	}
}

void QtGraphNode::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
	if (m_data)
	{
		bool isActive = m_isActive;
		this->setIsActive(true);
		m_isActive = isActive;
	}
}

void QtGraphNode::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	this->setIsActive(m_isActive);
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

	if (parentNode != NULL)
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

	for (const std::shared_ptr<GraphNode>& node : m_subNodes)
	{
		if (!node->isHidden())
		{
			node->setPosition(this->getPosition() + Vec2i(m_padding.x, nextYPos));
			nextYPos += node->getSize().y + m_padding.y;

			int tmpWidth = (node->getPosition().x - getPosition().x) + node->getSize().x + m_padding.x;

			if (tmpWidth > newWidth)
			{
				newWidth = tmpWidth;
			}
		}
	}

	m_currentSize.x = newWidth;
	m_currentSize.y = nextYPos;

	this->setRect(0, 0, m_currentSize.x, m_currentSize.y);
}
