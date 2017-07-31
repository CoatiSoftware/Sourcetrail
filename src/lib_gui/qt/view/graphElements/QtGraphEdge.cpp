#include "qt/view/graphElements/QtGraphEdge.h"

#include <QGraphicsSceneEvent>
#include <QGraphicsItemGroup>

#include "component/view/GraphViewStyle.h"
#include "data/graph/Edge.h"
#include "data/graph/token_component/TokenComponentAggregation.h"
#include "data/graph/token_component/TokenComponentInheritanceChain.h"
#include "qt/graphics/QtLineItemAngled.h"
#include "qt/graphics/QtLineItemBezier.h"
#include "qt/graphics/QtLineItemStraight.h"
#include "qt/view/graphElements/QtGraphNode.h"
#include "utility/messaging/type/MessageActivateEdge.h"
#include "utility/messaging/type/MessageActivateTrailEdge.h"
#include "utility/messaging/type/MessageFocusIn.h"
#include "utility/messaging/type/MessageFocusOut.h"
#include "utility/messaging/type/MessageGraphNodeBundleSplit.h"
#include "utility/messaging/type/MessageTooltipShow.h"
#include "utility/messaging/type/MessageTooltipHide.h"
#include "utility/utility.h"

QtGraphEdge* QtGraphEdge::s_focusedEdge = nullptr;
QtGraphEdge* QtGraphEdge::s_focusedBezierEdge = nullptr;

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
	, m_useBezier(false)
	, m_mousePos(0.0f, 0.0f)
	, m_mouseMoved(false)
{
	if (m_direction == TokenComponentAggregation::DIRECTION_BACKWARD)
	{
		m_owner.swap(m_target);
	}

	m_fromActive = m_owner.lock()->getIsActive();
	m_toActive = m_target.lock()->getIsActive();

	s_focusedEdge = nullptr;
	s_focusedBezierEdge = nullptr;
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

	Edge::EdgeType type = (getData() ? getData()->getType() : Edge::EDGE_AGGREGATION);
	GraphViewStyle::EdgeStyle style = GraphViewStyle::getStyleForEdgeType(type, m_isActive | m_isFocused, false, m_isTrailEdge);

	if (m_useBezier)
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

			QtLineItemStraight* line = new QtLineItemStraight(this);
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

		if (getData())
		{
			TokenComponentInheritanceChain* componentInheritance = getData()->getComponent<TokenComponentInheritanceChain>();
			if (componentInheritance && componentInheritance->inheritanceEdgeIds.size() > 1)
			{
				style.dashed = true;
			}
		}

		child->updateLine(
			owner->getBoundingRect(), target->getBoundingRect(),
			owner->getParentBoundingRect(), target->getParentBoundingRect(),
			style, m_weight, showArrow);
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

void QtGraphEdge::setIsFocused(bool isFocused)
{
	if (m_isFocused != isFocused)
	{
		m_isFocused = isFocused;
		updateLine();
	}
}

void QtGraphEdge::onClick()
{
	if (isTrailEdge())
	{
		MessageActivateTrailEdge(
			getData()->getId(),
			getData()->getType(),
			getData()->getFrom()->getNameHierarchy(),
			getData()->getTo()->getNameHierarchy()
		).dispatch();
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
		TokenComponentInheritanceChain* componentInheritance = getData()->getComponent<TokenComponentInheritanceChain>();

		MessageActivateEdge msg(
			getData()->getId(),
			componentInheritance ? Edge::EDGE_AGGREGATION : getData()->getType(),
			getData()->getFrom()->getNameHierarchy(),
			getData()->getTo()->getNameHierarchy()
		);

		if (getData()->getType() == Edge::EDGE_AGGREGATION)
		{
			msg.aggregationIds =
				utility::toVector<Id>(getData()->getComponent<TokenComponentAggregation>()->getAggregationIds());
		}
		else if (componentInheritance)
		{
			msg.aggregationIds = componentInheritance->inheritanceEdgeIds;
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

		if (s_focusedEdge == this)
		{
			Edge::EdgeType type = (getData() ? getData()->getType() : Edge::EDGE_AGGREGATION);

			TooltipInfo info;
			info.title = Edge::getReadableTypeString(type);
			if (type == Edge::EDGE_AGGREGATION)
			{
				info.count = m_weight;
				info.countText = "edge";
			}
			info.offset = Vec2i(10, 20);

			MessageTooltipShow(info, TOOLTIP_ORIGIN_GRAPH).dispatch();
		}
	}
}

void QtGraphEdge::focusOut()
{
	if (m_isFocused)
	{
		m_isFocused = false;
		updateLine();

		if (s_focusedEdge == this)
		{
			MessageTooltipHide().dispatch();
		}
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
	if (m_useBezier)
	{
		if (s_focusedBezierEdge && s_focusedBezierEdge != this)
		{
			s_focusedBezierEdge->focusOut();
		}

		s_focusedBezierEdge = this;
	}

	s_focusedEdge = this;

	if (getData() && !m_useBezier)
	{
		MessageFocusIn(std::vector<Id>(1, getData()->getId()), TOOLTIP_ORIGIN_GRAPH).dispatch();
	}
	else
	{
		focusIn();
	}
}

void QtGraphEdge::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	s_focusedBezierEdge = nullptr;

	if (getData() && !m_useBezier)
	{
		MessageFocusOut(std::vector<Id>(1, getData()->getId())).dispatch();
	}
	else
	{
		focusOut();
	}

	s_focusedEdge = nullptr;
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
	m_useBezier = true;
	m_isHorizontalTrail = horizontal;
}

void QtGraphEdge::setUseBezier(bool useBezier)
{
	m_useBezier = useBezier;
	m_isHorizontalTrail = true;
}
