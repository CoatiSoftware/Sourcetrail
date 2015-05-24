#include "qt/view/graphElements/QtGraphEdge.h"

#include <QGraphicsSceneEvent>

#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageFocusIn.h"
#include "utility/messaging/type/MessageFocusOut.h"

#include "component/view/GraphViewStyle.h"
#include "data/graph/Edge.h"
#include "data/graph/token_component/TokenComponentAggregation.h"
#include "qt/graphics/QtAngledLineItem.h"
#include "qt/graphics/QtStraightLineItem.h"
#include "qt/view/graphElements/QtGraphNode.h"

QtGraphEdge::QtGraphEdge(const std::weak_ptr<QtGraphNode>& owner, const std::weak_ptr<QtGraphNode>& target, const Edge* data)
	: m_data(data)
	, m_owner(owner)
	, m_target(target)
	, m_child(nullptr)
	, m_isActive(false)
	, m_mousePos(0.0f, 0.0f)
	, m_mouseMoved(false)
{
	this->updateLine();
}

QtGraphEdge::~QtGraphEdge()
{
}

const Edge* QtGraphEdge::getData() const
{
	return m_data;
}

std::weak_ptr<QtGraphNode> QtGraphEdge::getOwner()
{
	return m_owner;
}

std::weak_ptr<QtGraphNode> QtGraphEdge::getTarget()
{
	return m_target;
}

void QtGraphEdge::updateLine()
{
	std::shared_ptr<QtGraphNode> owner = m_owner.lock();
	std::shared_ptr<QtGraphNode> target = m_target.lock();

	if (owner == NULL || target == NULL)
	{
		LOG_WARNING("Either the owner or the target node is null.");
		return;
	}

	GraphViewStyle::EdgeStyle style = GraphViewStyle::getStyleForEdgeType(getData()->getType(), m_isActive, false);

	if (style.isStraight)
	{
		if (!m_child)
		{
			m_child = new QtStraightLineItem(this);
		}

		int number = 0;
		if (getData()->isType(Edge::EDGE_AGGREGATION))
		{
			number = getData()->getComponent<TokenComponentAggregation>()->getAggregationCount();
		}

		dynamic_cast<QtStraightLineItem*>(m_child)->updateLine(
			owner->getBoundingRect(), target->getBoundingRect(), number, style);
	}
	else
	{
		if (!m_child)
		{
			m_child = new QtAngledLineItem(this);
		}

		dynamic_cast<QtAngledLineItem*>(m_child)->updateLine(
			owner->getBoundingRect(), target->getBoundingRect(),
			owner->getParentBoundingRect(), target->getParentBoundingRect(),
			style);
	}

	this->setZValue(style.zValue); // Used to draw edges always on top of nodes.
}

bool QtGraphEdge::getIsActive() const
{
	return m_isActive;
}

void QtGraphEdge::setIsActive(bool isActive)
{
	m_isActive = isActive;

	updateLine();
}

void QtGraphEdge::onClick()
{
	if (getData()->isType(Edge::EDGE_AGGREGATION))
	{
		const std::set<Id>& ids = getData()->getComponent<TokenComponentAggregation>()->getAggregationIds();
		MessageActivateTokens message(std::vector<Id>(ids.begin(), ids.end()));
		message.isAggregation = true;
		message.dispatch();
	}
	else
	{
		MessageActivateTokens message(getData()->getId());
		message.isEdge = true;
		message.dispatch();
	}
}

void QtGraphEdge::focusIn()
{
	bool isActive = m_isActive;
	this->setIsActive(true);
	m_isActive = isActive;
}

void QtGraphEdge::focusOut()
{
	this->setIsActive(m_isActive);
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
	MessageFocusIn(getData()->getId()).dispatch();
}

void QtGraphEdge::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	MessageFocusOut(getData()->getId()).dispatch();
}
