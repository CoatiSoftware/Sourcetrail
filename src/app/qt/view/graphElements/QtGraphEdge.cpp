#include "qt/view/graphElements/QtGraphEdge.h"

#include "qpen.h"

#include "component/view/graphElements/GraphNode.h"

QtGraphEdge::QtGraphEdge(const std::weak_ptr<GraphNode>& owner, const std::weak_ptr<GraphNode>& target)
	: m_owner(owner)
	, m_target(target)
{
	std::shared_ptr<GraphNode> o = owner.lock();
	std::shared_ptr<GraphNode> t = target.lock();

	if (o != NULL && t != NULL)
	{
		this->setLine(o->getPosition().x, o->getPosition().y,t->getPosition().x, t->getPosition().y);
	}
	else
	{
		LOG_WARNING("Either the owner or the target node is null.");
	}

	QPen blackPen(Qt::black);
    blackPen.setWidth(3);

	this->setPen(blackPen);
}

QtGraphEdge::~QtGraphEdge()
{
}

void QtGraphEdge::ownerMoved()
{
	std::shared_ptr<GraphNode> node = m_owner.lock();

	if (node != NULL)
	{
		this->setLine(node->getPosition().x, node->getPosition().y, this->line().x2(), this->line().y2());
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

	if(node != NULL)
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
