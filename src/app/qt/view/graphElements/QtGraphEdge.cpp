#include "qt/view/graphElements/QtGraphEdge.h"

#include <QGraphicsScene>
#include <QGraphicsSceneEvent>
#include <QPen>

#include "component/view/graphElements/GraphNode.h"

QtGraphEdge::QtGraphEdge(const std::weak_ptr<GraphNode>& owner, const std::weak_ptr<GraphNode>& target, const Edge* data)
	: GraphEdge(data)
	, m_owner(owner)
	, m_target(target)
	, m_isActive(false)
	, m_mousePos(0.0f, 0.0f)
	, m_mouseMoved(false)
{
	std::shared_ptr<GraphNode> o = owner.lock();
	std::shared_ptr<GraphNode> t = target.lock();

	Vec2i ownerPos;
	Vec2i targetPos;

	if (o != NULL && t != NULL)
	{
		ownerPos = o->getPosition();
		targetPos = t->getPosition();
	}
	else
	{
		LOG_WARNING("Either the owner or the target node is null.");
	}

	this->setLine(ownerPos.x, ownerPos.y, targetPos.x, targetPos.y);

	this->setAcceptHoverEvents(true);
	this->setZValue(1); // Used to draw edges always on top of nodes.

	QPen pen(Qt::transparent);
	pen.setWidth(10);
	this->setPen(pen);

	m_child = new QGraphicsLineItem(this);
	m_child->setLine(ownerPos.x, ownerPos.y, targetPos.x, targetPos.y);

	pen.setColor(Qt::black);
	pen.setWidth(1);

	switch (data->getType())
	{
	case Edge::EDGE_CALL:
		pen.setColor(QColor(100, 100, 100));
		break;
	case Edge::EDGE_USAGE:
		pen.setColor(QColor(66, 230, 103));
		break;
	case Edge::EDGE_TYPE_OF:
		pen.setColor(QColor(73, 155, 222));
		break;
	case Edge::EDGE_RETURN_TYPE_OF:
		pen.setColor(QColor(231, 65, 65));
		break;
	case Edge::EDGE_PARAMETER_TYPE_OF:
		pen.setColor(QColor(227, 180, 68));
		break;
	case Edge::EDGE_INHERITANCE:
		pen.setColor(QColor(113, 96, 191));
		break;
	default:
		break;
	}

	m_child->setPen(pen);
}

QtGraphEdge::~QtGraphEdge()
{
}

void QtGraphEdge::ownerMoved()
{
	std::shared_ptr<GraphNode> node = m_owner.lock();

	if (node != NULL)
	{
		Vec2i posA = node->getPosition();
		Vec2i posB(this->line().x2(), this->line().y2());

		this->setLine(posA.x, posA.y, posB.x, posB.y);
		m_child->setLine(posA.x, posA.y, posB.x, posB.y);
	}
	else
	{
		LOG_WARNING("Owner node is null.");
	}
}

void QtGraphEdge::targetMoved()
{
	std::shared_ptr<GraphNode> node = m_target.lock();

	if (node != NULL)
	{
		Vec2i posA(this->line().x1(), this->line().y1());
		Vec2i posB = node->getPosition();

		this->setLine(posA.x, posA.y, posB.x, posB.y);
		m_child->setLine(posA.x, posA.y, posB.x, posB.y);
	}
	else
	{
		LOG_WARNING("Target node is null.");
	}
}

void QtGraphEdge::removeEdgeFromScene()
{
	std::shared_ptr<GraphNode> node = m_owner.lock();

	if (node != NULL)
	{
		node->removeOutEdge(this);
	}
	else
	{
		LOG_WARNING("Target node is null.");
	}
}

std::weak_ptr<GraphNode> QtGraphEdge::getOwner()
{
	return m_owner;
}

std::weak_ptr<GraphNode> QtGraphEdge::getTarget()
{
	return m_target;
}

bool QtGraphEdge::getIsActive() const
{
	return m_isActive;
}

void QtGraphEdge::setIsActive(bool isActive)
{
	m_isActive = isActive;

	QPen p = m_child->pen();
	if (isActive)
	{
		p.setWidth(2);
	}
	else
	{
		p.setWidth(1);
	}
	m_child->setPen(p);
}

void QtGraphEdge::onClick()
{
	MessageActivateToken(getTokenId()).dispatch();
}

void QtGraphEdge::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	m_mousePos = Vec2i(event->scenePos().x(), event->scenePos().y());
	m_mouseMoved = false;
}

void QtGraphEdge::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	Vec2i mousePos = Vec2i(event->scenePos().x(), event->scenePos().y());

	if ((mousePos - m_mousePos).getLength() > 1.0f)
	{
		m_mouseMoved = true;
	}
}

void QtGraphEdge::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if (!m_mouseMoved)
	{
		this->onClick();
	}
}

void QtGraphEdge::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
	bool isActive = m_isActive;
	this->setIsActive(true);
	m_isActive = isActive;
}

void QtGraphEdge::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	this->setIsActive(m_isActive);
}
