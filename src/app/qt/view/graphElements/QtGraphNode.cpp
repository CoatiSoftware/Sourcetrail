#include "qt/view/graphElements/QtGraphNode.h"

#include <QBrush>
#include <QGraphicsSceneEvent>
#include <QPen>

#include "utility/messaging/type/MessageActivateToken.h"
#include "utility/messaging/type/MessageGraphNodeMove.h"

#include "qt/graphics/QtGraphicsRoundedRectItem.h"
#include "qt/utility/QtDeviceScaledPixmap.h"
#include "qt/view/graphElements/nodeComponents/QtGraphNodeComponent.h"
#include "qt/view/graphElements/QtGraphEdge.h"

QFont QtGraphNode::getFontForNodeType(Node::NodeType type)
{
	QFont font("Source Code Pro");

	switch (type)
	{
	case Node::NODE_UNDEFINED:
	case Node::NODE_NAMESPACE:
		font.setPointSize(12);
		break;

	case Node::NODE_UNDEFINED_TYPE:
	case Node::NODE_STRUCT:
	case Node::NODE_CLASS:
	case Node::NODE_ENUM:
	case Node::NODE_TYPEDEF:
		font.setPointSize(14);
		break;

	case Node::NODE_UNDEFINED_FUNCTION:
	case Node::NODE_FUNCTION:
	case Node::NODE_METHOD:
		font.setPointSize(11);
		break;

	case Node::NODE_UNDEFINED_VARIABLE:
	case Node::NODE_GLOBAL_VARIABLE:
	case Node::NODE_FIELD:
		font.setPointSize(11);
		break;
	}

	return font;
}

QtGraphNode::QtGraphNode()
	: GraphNode(nullptr)
	, m_undefinedRect(nullptr)
	, m_isActive(false)
	, m_isHovering(false)
{
	this->setPen(QPen(Qt::transparent));

	m_rect = new QtGraphicsRoundedRectItem(this);
	m_text = new QGraphicsSimpleTextItem(this);
}

QtGraphNode::QtGraphNode(const Node* data)
	: GraphNode(data)
	, m_undefinedRect(nullptr)
	, m_isActive(false)
	, m_isHovering(false)
{
	this->setPen(QPen(Qt::transparent));

	m_rect = new QtGraphicsRoundedRectItem(this);
	m_text = new QGraphicsSimpleTextItem(this);

	this->setAcceptHoverEvents(true);

	this->setName(data->getName());
}

QtGraphNode::~QtGraphNode()
{
}

std::string QtGraphNode::getName() const
{
	return m_text->text().toStdString();
}

void QtGraphNode::setName(const std::string& name)
{
	m_text->setText(QString::fromStdString(name));
}

Vec2i QtGraphNode::getPosition() const
{
	return Vec2i(this->scenePos().x(), this->scenePos().y());
}

bool QtGraphNode::setPosition(const Vec2i& position)
{
	Vec2i currentPosition = getPosition();
	Vec2i offset = position - currentPosition;

	if (offset.getLength() > 0.0f)
	{
		this->moveBy(offset.x, offset.y);
		notifyEdgesAfterMove();
		return true;
	}

	return false;
}

void QtGraphNode::moveTo(const Vec2i& position)
{
	if (setPosition(position) && m_data)
	{
		MessageGraphNodeMove(m_data->getId(), position).dispatch();
	}
}

Vec2i QtGraphNode::getSize() const
{
	return m_size;
}

void QtGraphNode::setSize(const Vec2i& size)
{
	m_size = size;

	this->setRect(0, 0, size.x, size.y);
	m_rect->setRect(0, 0, size.x, size.y);

	if (m_undefinedRect)
	{
		m_undefinedRect->setRect(1, 1, size.x - 2, size.y - 2);
	}
}

Vec4i QtGraphNode::getBoundingRect() const
{
	Vec2i pos = getPosition();
	Vec2i size = getSize();
	return Vec4i(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
}

Vec4i QtGraphNode::getParentBoundingRect() const
{
	const QtGraphNode* node = this;
	while (true)
	{
		const QtGraphNode* parent = dynamic_cast<QtGraphNode*>(node->parentItem());
		if (!parent)
		{
			break;
		}
		node = parent;

	}
	return node->getBoundingRect();
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

unsigned int QtGraphNode::getOutEdgeCount() const
{
	return m_outEdges.size();
}

unsigned int QtGraphNode::getInEdgeCount() const
{
	return m_inEdges.size();
}

bool QtGraphNode::getIsActive() const
{
	return m_isActive;
}

void QtGraphNode::setIsActive(bool isActive)
{
	m_isActive = isActive;

	setStyle();
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

std::list<std::shared_ptr<QtGraphNode>> QtGraphNode::getSubNodes() const
{
	return m_subNodes;
}

void QtGraphNode::addSubNode(const std::shared_ptr<QtGraphNode>& node)
{
	m_subNodes.push_back(node);
}

void QtGraphNode::setStyle()
{
	if (!m_data)
	{
		return;
	}

	QColor color = Qt::lightGray;
	qreal radius = 0.0f;
	QPen p(Qt::transparent);
	QFont font(getFontForNodeType(m_data->getType()));

	bool undefined = false;
	bool useUndefinedColor = false;

	Vec2i padding;

	switch (m_data->getType())
	{
	case Node::NODE_UNDEFINED:
		undefined = true;
	case Node::NODE_NAMESPACE:
		color = Qt::white;

		p.setColor("#cc8d91");
		p.setWidth(1);

		radius = 20.0f;
		padding.x = 15;
		padding.y = 6;
		break;

	case Node::NODE_UNDEFINED_TYPE:
		undefined = true;
	case Node::NODE_STRUCT:
	case Node::NODE_CLASS:
	case Node::NODE_ENUM:
	case Node::NODE_TYPEDEF:
		if (m_isHovering)
		{
			m_rect->setShadow(QColor(0, 0, 0, 255), 5);
		}
		else
		{
			m_rect->setShadow(QColor(0, 0, 0, 128), 5);
		}

		color = "#ededed";
		if (m_subNodes.size())
		{
			radius = 20.0f;
			padding.x = 15;
			padding.y = 8;
		}
		else
		{
			radius = 10.0f;
			padding.x = 8;
			padding.y = 4;
		}
		break;

	case Node::NODE_UNDEFINED_FUNCTION:
		undefined = true;
		useUndefinedColor = true;
	case Node::NODE_FUNCTION:
	case Node::NODE_METHOD:
		if (m_isActive || m_isHovering)
		{
			color = "#ffcc00";
			font.setWeight(QFont::Bold);
		}
		else
		{
			color = "#ffe47a";
		}

		radius = 8.0f;
		padding.x = 5;
		padding.y = 3;
		break;

	case Node::NODE_UNDEFINED_VARIABLE:
		undefined = true;
		useUndefinedColor = true;
	case Node::NODE_GLOBAL_VARIABLE:
	case Node::NODE_FIELD:
		if (m_isActive || m_isHovering)
		{
			color = "#62b29d";
			font.setWeight(QFont::Bold);
		}
		else
		{
			color = "#9ab4ad";
		}

		radius = 8.0f;
		padding.x = 5;
		padding.y = 3;
		break;
	}

	if (m_isActive)
	{
		p.setWidthF(1.5f);
		p.setColor("#3c3c3c");
	}

	m_rect->setPen(p);
	m_rect->setBrush(QBrush(color));
	m_rect->setRadius(radius);

	if (undefined)
	{
		QtDeviceScaledPixmap pixmap("data/gui/graph_view/images/pattern.png");
		pixmap.scaleToHeight(10);

		if (!m_undefinedRect)
		{
			m_undefinedRect = new QtGraphicsRoundedRectItem(this);
			setSize(getSize());
		}

		p.setWidth(1);
		p.setColor(Qt::transparent);
		if (useUndefinedColor && !m_isActive)
		{
			p.setColor(color);
		}
		m_undefinedRect->setPen(p);
		m_undefinedRect->setBrush(QBrush(pixmap.pixmap()));
		m_undefinedRect->setRadius(radius);
	}

	m_text->setFont(font);
	m_text->setPos(padding.x, padding.y);
}

void QtGraphNode::onClick()
{
	if (m_data && !m_data->isType(Node::NODE_UNDEFINED | Node::NODE_NAMESPACE))
	{
		MessageActivateToken(m_data->getId()).dispatch();
	}
}

void QtGraphNode::hoverEnter()
{
	hoverEnterEvent(nullptr);

	QtGraphNode* parent = getParent();
	if (parent)
	{
		parent->hoverEnter();
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
	m_isHovering = true;
	setStyle();
}

void QtGraphNode::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	m_isHovering = false;
	setStyle();
}

void QtGraphNode::notifyEdgesAfterMove()
{
	for (const std::shared_ptr<GraphEdge>& edge : m_outEdges)
	{
		edge->updateLine();
	}

	for (const std::weak_ptr<GraphEdge>& e : m_inEdges)
	{
		std::shared_ptr<GraphEdge> edge = e.lock();
		if (edge)
		{
			edge->updateLine();
		}
	}

	for (const std::shared_ptr<QtGraphNode>& node : m_subNodes)
	{
		node->notifyEdgesAfterMove();
	}
}
