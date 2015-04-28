#include "qt/view/graphElements/QtGraphNode.h"

#include <QBrush>
#include <QGraphicsSceneEvent>
#include <QPen>

#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageGraphNodeMove.h"

#include "qt/graphics/QtRoundedRectItem.h"
#include "qt/utility/QtDeviceScaledPixmap.h"
#include "qt/utility/QtGraphPostprocessor.h"
#include "qt/view/graphElements/nodeComponents/QtGraphNodeComponent.h"
#include "qt/view/graphElements/QtGraphEdge.h"

void QtGraphNode::blendIn()
{
	setOpacity(1.0f);
}

void QtGraphNode::blendOut()
{
	setOpacity(0.0f);
}

void QtGraphNode::showNode()
{
	this->show();
}

void QtGraphNode::hideNode()
{
	this->hide();
}

QFont QtGraphNode::getFontForNodeType(Node::NodeType type)
{
	QFont font(GraphViewStyle::getFontNameForNodeType(type).c_str());
	font.setPixelSize(GraphViewStyle::getFontSizeForNodeType(type));
	return font;
}

QtGraphNode::QtGraphNode()
	: GraphNode(nullptr)
	, m_undefinedRect(nullptr)
	, m_isActive(false)
	, m_isHovering(false)
	, m_childVisible(false)
{
	this->setPen(QPen(Qt::transparent));

	m_rect = new QtRoundedRectItem(this);
	m_text = new QGraphicsSimpleTextItem(this);
}

QtGraphNode::QtGraphNode(const Node* data, bool childVisible)
	: GraphNode(data)
	, m_undefinedRect(nullptr)
	, m_isActive(false)
	, m_isHovering(false)
	, m_childVisible(childVisible)
{
	this->setPen(QPen(Qt::transparent));

	m_rect = new QtRoundedRectItem(this);
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

bool QtGraphNode::isAccessNode() const
{
	return false;
}

bool QtGraphNode::isExpandToggleNode() const
{
	return false;
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

void QtGraphNode::moved()
{
	QtGraphPostprocessor::alignNodeOnRaster(this);

	if (m_data)
	{
		MessageGraphNodeMove(m_data->getId(), getPosition()).dispatch();
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

void QtGraphNode::addOutEdge(const std::shared_ptr<GraphEdge>& edge)
{
	m_outEdges.push_back(edge);
}

void QtGraphNode::addInEdge(const std::weak_ptr<GraphEdge>& edge)
{
	m_inEdges.push_back(edge);
}

size_t QtGraphNode::getOutEdgeCount() const
{
	return m_outEdges.size();
}

size_t QtGraphNode::getInEdgeCount() const
{
	return m_inEdges.size();
}

QSize QtGraphNode::size() const
{
	return QSize(m_size.x, m_size.y);
}

void QtGraphNode::setSize(const QSize& size)
{
	setSize(Vec2i(size.width(), size.height()));
}

bool QtGraphNode::getIsActive() const
{
	return m_isActive;
}

void QtGraphNode::setIsActive(bool isActive)
{
	m_isActive = isActive;

	updateStyle();
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

void QtGraphNode::setShadowEnabledRecursive(bool enabled)
{
	m_rect->setShadowEnabled(enabled);

	for (const std::shared_ptr<QtGraphNode>& node : m_subNodes)
	{
		node->setShadowEnabledRecursive(enabled);
	}
}

void QtGraphNode::onClick()
{
	if (m_data && !m_data->isType(Node::NODE_UNDEFINED | Node::NODE_NAMESPACE))
	{
		MessageActivateTokens(m_data->getId()).dispatch();
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

void QtGraphNode::updateStyle()
{
	GraphViewStyle::NodeStyle style =
		GraphViewStyle::getStyleForNodeType(m_data->getType(), m_isActive, m_isHovering, m_childVisible);
	setStyle(style);
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
	updateStyle();
}

void QtGraphNode::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	m_isHovering = false;
	updateStyle();
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

void QtGraphNode::setStyle(const GraphViewStyle::NodeStyle& style)
{
	QColor color = style.color.c_str();
	qreal radius = style.cornerRadius;

	QPen p(style.borderColor.c_str());
	p.setWidthF(style.borderWidth);
	if (style.borderDashed)
	{
		p.setStyle(Qt::DashLine);
	}

	QFont font(style.fontName.c_str());
	font.setPixelSize(style.fontSize);
	if (style.fontBold)
	{
		font.setWeight(QFont::Bold);
	}

	if (style.shadowColor.size())
	{
		m_rect->setShadow(style.shadowColor.c_str(), style.shadowBlurRadius);
	}

	m_rect->setPen(p);
	m_rect->setBrush(QBrush(color));
	m_rect->setRadius(radius);

	if (style.undefinedPattern)
	{
		QtDeviceScaledPixmap pixmap("data/gui/graph_view/images/pattern.png");
		pixmap.scaleToHeight(10);

		if (!m_undefinedRect)
		{
			m_undefinedRect = new QtRoundedRectItem(this);
			setSize(getSize());
		}

		p.setWidth(0);
		p.setColor(Qt::transparent);

		m_undefinedRect->setPen(p);
		m_undefinedRect->setBrush(QBrush(pixmap.pixmap()));
		m_undefinedRect->setRadius(radius);
	}

	m_text->setFont(font);
	m_text->setPos(style.textOffset.x, style.textOffset.y);
}
