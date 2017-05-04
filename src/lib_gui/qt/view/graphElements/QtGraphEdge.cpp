#include "qt/view/graphElements/QtGraphEdge.h"

#include <QGraphicsSceneEvent>
#include <QGraphicsItemGroup>
#include <QTimer>

#include "component/view/GraphViewStyle.h"
#include "data/graph/Edge.h"
#include "data/graph/token_component/TokenComponentAggregation.h"
#include "qt/graphics/QtLineItemAngled.h"
#include "qt/graphics/QtLineItemBezier.h"
#include "qt/graphics/QtLineItemStraight.h"
#include "qt/view/graphElements/QtGraphNode.h"
#include "utility/messaging/type/MessageActivateEdge.h"
#include "utility/messaging/type/MessageFocusIn.h"
#include "utility/messaging/type/MessageFocusOut.h"
#include "utility/messaging/type/MessageGraphNodeBundleSplit.h"
#include "utility/utility.h"
#include "utility/utilityString.h"

QtGraphEdge::QtGraphEdge(
	const std::weak_ptr<QtGraphNode>& owner,
	const std::weak_ptr<QtGraphNode>& target,
	const Edge* data,
	size_t weight,
	bool isActive,
	TokenComponentAggregation::Direction direction
)
	: m_data(data)
	, m_owner(owner)
	, m_target(target)
	, m_child(nullptr)
	, m_isActive(isActive)
	, m_fromActive(false)
	, m_toActive(false)
	, m_isFocused(false)
	, m_weight(weight)
	, m_direction(direction)
	, m_isTrailEdge(false)
	, m_isHorizontalTrail(false)
	, m_mousePos(0.0f, 0.0f)
	, m_mouseMoved(false)
	, m_willFocusIn(false)
	, m_ignoreFocusIn(false)
{
	if (m_direction == TokenComponentAggregation::DIRECTION_BACKWARD)
	{
		m_owner.swap(m_target);
	}

	m_fromActive = m_owner.lock()->getIsActive();
	m_toActive = m_target.lock()->getIsActive();
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

	Edge::EdgeType type;
	if (getData())
	{
		type = getData()->getType();
	}
	else
	{
		type = Edge::EDGE_AGGREGATION;
	}

	QString toolTip = Edge::getReadableTypeString(type).c_str();
	if (type == Edge::EDGE_AGGREGATION)
	{
		toolTip += ": " + QString::number(m_weight) + " edge";
		if (m_weight != 1)
		{
			toolTip += "s";
		}
	}

	GraphViewStyle::EdgeStyle style = GraphViewStyle::getStyleForEdgeType(type, m_isActive | m_isFocused, false, m_isTrailEdge);

	if (m_isTrailEdge)
	{
		for (QGraphicsItem* item : childItems())
		{
			item->hide();
			item->setParentItem(nullptr);
		}

		style.originOffset.y() = 0;
		style.targetOffset.y() = 0;

		Vec4i ownerRect = owner->getBoundingRect();
		Vec4i ownerParentRect = owner->getParentBoundingRect();

		QtLineItemBase::Route route =
			m_isHorizontalTrail ? QtLineItemBase::ROUTE_HORIZONTAL : QtLineItemBase::ROUTE_VERTICAL;

		for (const Vec4i& rect : m_path)
		{
			QtLineItemBezier* bezier = new QtLineItemBezier(this);
			bezier->updateLine(ownerRect, rect, ownerParentRect, rect, style, m_weight, false);
			bezier->setRoute(route);
			bezier->setPivot(QtLineItemBase::PIVOT_MIDDLE);
			bezier->setToolTip(toolTip);

			QtLineItemStraight* line = new QtLineItemStraight(this);
			line->setToolTip(toolTip);
			if (route == QtLineItemBase::ROUTE_HORIZONTAL)
			{
				line->updateLine(Vec2i(rect.x(), (rect.y() + rect.w()) / 2), Vec2i(rect.z(), (rect.y() + rect.w()) / 2), style);
			}
			else
			{
				line->updateLine(Vec2i((rect.x() + rect.z()) / 2, rect.y()), Vec2i((rect.x() + rect.z()) / 2, rect.w()), style);
			}

			ownerRect = rect;
			ownerParentRect = rect;
		}

		bool showArrow = m_direction != TokenComponentAggregation::DIRECTION_NONE;

		QtLineItemBezier* bezier = new QtLineItemBezier(this);
		bezier->updateLine(
			ownerRect, target->getBoundingRect(), ownerParentRect, target->getParentBoundingRect(),
			style, m_weight, showArrow);
		bezier->setRoute(route);
		bezier->setPivot(QtLineItemBase::PIVOT_MIDDLE);
		bezier->setToolTip(toolTip);

		if (owner->getLastParent() == target->getLastParent())
		{
			if (ownerRect.y() < target->getBoundingRect().y())
			{
				bezier->setOnBack(true);
			}
			else
			{
				bezier->setOnFront(true);
			}
		}
	}
	else
	{
		if (!m_child)
		{
			m_child = new QtLineItemAngled(this);
		}

		QtLineItemAngled* child = dynamic_cast<QtLineItemAngled*>(m_child);

		if (m_fromActive && owner->getLastParent() == target->getLastParent())
		{
			child->setOnBack(true);
		}

		if (m_toActive)
		{
			child->setHorizontalIn(true);

			if (owner->getLastParent() == target->getLastParent())
			{
				child->setOnFront(true);
			}
		}

		if (type != Edge::EDGE_INHERITANCE &&
			(type != Edge::EDGE_AGGREGATION ||
				owner.get() != owner->getLastParent() || target.get() != target->getLastParent()))
		{
			child->setRoute(QtLineItemBase::ROUTE_HORIZONTAL);
		}

		bool showArrow = true;
		if (type == Edge::EDGE_AGGREGATION)
		{
			child->setPivot(QtLineItemBase::PIVOT_MIDDLE);

			showArrow = m_direction != TokenComponentAggregation::DIRECTION_NONE;
		}

		child->updateLine(
			owner->getBoundingRect(), target->getBoundingRect(),
			owner->getParentBoundingRect(), target->getParentBoundingRect(),
			style, m_weight, showArrow);

		child->setToolTip(toolTip);
	}

	this->setZValue(style.zValue);
}

bool QtGraphEdge::getIsActive() const
{
	return m_isActive;
}

void QtGraphEdge::setIsActive(bool isActive)
{
	if (m_isActive != isActive)
	{
		m_isActive = isActive;
		updateLine();
	}
}

void QtGraphEdge::onClick()
{
	if (isTrailEdge())
	{
		setIsActive(!getIsActive());
		return;
	}

	if (!getData())
	{
		std::weak_ptr<QtGraphNode> node =
			(m_direction == TokenComponentAggregation::DIRECTION_BACKWARD ? m_owner : m_target);
		MessageGraphNodeBundleSplit(node.lock()->getTokenId()).dispatch();
	}
	else
	{
		MessageActivateEdge msg(
			getData()->getId(),
			getData()->getType(),
			getData()->getFrom()->getNameHierarchy(),
			getData()->getTo()->getNameHierarchy()
		);

		if (getData()->getType() == Edge::EDGE_AGGREGATION)
		{
			msg.aggregationIds =
				utility::toVector<Id>(getData()->getComponent<TokenComponentAggregation>()->getAggregationIds());
		}

		msg.dispatch();
	}
}

void QtGraphEdge::focusIn()
{
	if (!m_isFocused)
	{
		m_isFocused = true;
		updateLine();
	}
}

void QtGraphEdge::focusOut()
{
	if (m_isFocused)
	{
		m_isFocused = false;
		updateLine();
	}
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
	if (!m_ignoreFocusIn)
	{
		QTimer::singleShot(50, this, SLOT(doFocusIn()));
		m_ignoreFocusIn = true;
		m_willFocusIn = true;
	}
}

void QtGraphEdge::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	m_willFocusIn = false;
	QTimer::singleShot(100, this, SLOT(doFocusOut()));

	if (!getData())
	{
		focusOut();
		return;
	}

	MessageFocusOut(std::vector<Id>(1, getData()->getId())).dispatch();
}

void QtGraphEdge::doFocusIn()
{
	if (!m_willFocusIn)
	{
		return;
	}

	if (!getData() || isTrailEdge())
	{
		focusIn();
		return;
	}

	MessageFocusIn(std::vector<Id>(1, getData()->getId())).dispatch();
}

void QtGraphEdge::doFocusOut()
{
	m_ignoreFocusIn = false;
}

void QtGraphEdge::setDirection(TokenComponentAggregation::Direction direction)
{
	if (m_direction != direction)
	{
		m_direction = direction;
		updateLine();
	}
}

bool QtGraphEdge::isTrailEdge() const
{
	return m_isTrailEdge;
}

void QtGraphEdge::setIsTrailEdge(std::vector<Vec4i> path, bool horizontal)
{
	m_path = path;
	m_isTrailEdge = true;
	m_isHorizontalTrail = horizontal;
}
