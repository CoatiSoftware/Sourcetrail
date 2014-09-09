#include "qt/view/graphElements/QtGraphEdge.h"

#include <QGraphicsScene>
#include <QPen>

#include "component/view/graphElements/GraphNode.h"

QtGraphEdge::QtGraphEdge(const std::weak_ptr<GraphNode>& owner, const std::weak_ptr<GraphNode>& target, const Id id)
	: GraphEdge(id)
	, m_owner(owner)
	, m_target(target)
	, m_color(0, 0, 0, 0)
{
	std::shared_ptr<GraphNode> o = owner.lock();
	std::shared_ptr<GraphNode> t = target.lock();

	if (o != NULL && t != NULL)
	{
		Vec2i ownerPos = o->getPosition();
		this->setLine(ownerPos.x, ownerPos.y,t->getPosition().x, t->getPosition().y);
	}
	else
	{
		LOG_WARNING("Either the owner or the target node is null.");
	}

	QPen blackPen(Qt::black);
    blackPen.setWidth(2);

	this->setPen(blackPen);
	this->setZValue(1); // Used to draw edges always on top of nodes.
}

QtGraphEdge::~QtGraphEdge()
{
}

void QtGraphEdge::ownerMoved()
{
	std::shared_ptr<GraphNode> node = m_owner.lock();

	if (node != NULL)
	{
		Vec2i pos = node->getPosition();
		this->setLine(pos.x, pos.y, this->line().x2(), this->line().y2());
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
		this->setLine(this->line().x1(), this->line().y1(), node->getPosition().x, node->getPosition().y);
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

void QtGraphEdge::setColor(const Vec4i& color)
{
	m_color = color;

	QPen pen(QColor(color.x, color.y, color.z, color.w));
	pen.setWidth(2);
	this->setPen(pen);
}

Vec4i QtGraphEdge::getColor() const
{
	return m_color;
}

void QtGraphEdge::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	MessageActivateToken message(m_tokenId);
	message.dispatch();
}
