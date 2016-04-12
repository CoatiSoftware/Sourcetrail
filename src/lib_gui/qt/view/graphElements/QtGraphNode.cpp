#include "qt/view/graphElements/QtGraphNode.h"

#include <QBrush>
#include <QFont>
#include <QGraphicsSceneEvent>
#include <QPen>

#include "utility/ResourcePaths.h"

#include "component/controller/helper/GraphPostprocessor.h"

#include "qt/graphics/QtRoundedRectItem.h"
#include "qt/utility/QtDeviceScaledPixmap.h"
#include "qt/utility/utilityQt.h"
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
	: m_undefinedRect(nullptr)
	, m_icon(nullptr)
	, m_isActive(false)
	, m_multipleActive(false)
	, m_isHovering(false)
{
	this->setPen(QPen(Qt::transparent));

	m_rect = new QtRoundedRectItem(this);
	m_text = new QGraphicsSimpleTextItem(this);
}

QtGraphNode::~QtGraphNode()
{
}

QtGraphNode* QtGraphNode::getParent() const
{
	return m_parentNode.lock().get();
}

QtGraphNode* QtGraphNode::getLastParent() const
{
	QtGraphNode* node = const_cast<QtGraphNode*>(this);
	while (true)
	{
		QtGraphNode* parent = dynamic_cast<QtGraphNode*>(node->parentItem());
		if (!parent)
		{
			break;
		}
		node = parent;
	}
	return node;
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

std::list<std::shared_ptr<QtGraphNode>> QtGraphNode::getSubNodes() const
{
	return m_subNodes;
}

Vec2i QtGraphNode::getPosition() const
{
	return Vec2i(this->scenePos().x(), this->scenePos().y());
}

bool QtGraphNode::setPosition(const Vec2i& position)
{
	Vec2i currentPosition = getPosition();
	Vec2i offset = position - currentPosition;

	if (offset.x != 0 || offset.y != 0)
	{
		this->moveBy(offset.x, offset.y);
		notifyEdgesAfterMove();
		return true;
	}

	return false;
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

QSize QtGraphNode::size() const
{
	return QSize(m_size.x, m_size.y);
}

void QtGraphNode::setSize(const QSize& size)
{
	setSize(Vec2i(size.width(), size.height()));
}

Vec4i QtGraphNode::getBoundingRect() const
{
	Vec2i pos = getPosition();
	Vec2i size = getSize();
	return Vec4i(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
}

Vec4i QtGraphNode::getParentBoundingRect() const
{
	return getLastParent()->getBoundingRect();
}

void QtGraphNode::addOutEdge(const std::shared_ptr<QtGraphEdge>& edge)
{
	m_outEdges.push_back(edge);
}

void QtGraphNode::addInEdge(const std::weak_ptr<QtGraphEdge>& edge)
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

bool QtGraphNode::getIsActive() const
{
	return m_isActive;
}

void QtGraphNode::setIsActive(bool isActive)
{
	m_isActive = isActive;

	updateStyle();
}

void QtGraphNode::setMultipleActive(bool multipleActive)
{
	m_multipleActive = multipleActive;
}

std::string QtGraphNode::getName() const
{
	return m_text->text().toStdString();
}

void QtGraphNode::setName(const std::string& name)
{
	m_text->setText(QString::fromStdString(name));
}

void QtGraphNode::addComponent(const std::shared_ptr<QtGraphNodeComponent>& component)
{
	m_components.push_back(component);
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

void QtGraphNode::focusIn()
{
	m_isHovering = true;
	updateStyle();
}

void QtGraphNode::focusOut()
{
	m_isHovering = false;
	updateStyle();
}

bool QtGraphNode::isDataNode() const
{
	return false;
}

bool QtGraphNode::isAccessNode() const
{
	return false;
}

bool QtGraphNode::isExpandToggleNode() const
{
	return false;
}

bool QtGraphNode::isBundleNode() const
{
	return false;
}

Id QtGraphNode::getTokenId() const
{
	return 0;
}

void QtGraphNode::addSubNode(const std::shared_ptr<QtGraphNode>& node)
{
	m_subNodes.push_back(node);
}

void QtGraphNode::moved(const Vec2i& oldPosition)
{
	setPosition(GraphPostprocessor::alignOnRaster(getPosition()));
}

void QtGraphNode::onClick()
{
}

void QtGraphNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	event->ignore();

	if (event->button() != Qt::LeftButton)
	{
		return;
	}

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

	if (event->button() != Qt::LeftButton)
	{
		return;
	}

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

void QtGraphNode::notifyEdgesAfterMove()
{
	for (const std::shared_ptr<QtGraphEdge>& edge : m_outEdges)
	{
		edge->updateLine();
	}

	for (const std::weak_ptr<QtGraphEdge>& e : m_inEdges)
	{
		std::shared_ptr<QtGraphEdge> edge = e.lock();
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
	QPen pen(Qt::transparent);
	if (style.borderWidth > 0)
	{
		pen.setColor(style.color.border.c_str());
		pen.setWidthF(style.borderWidth);
		if (style.borderDashed)
		{
			pen.setStyle(Qt::DashLine);
		}
	}

	m_rect->setPen(pen);
	m_rect->setBrush(QBrush(style.color.fill.c_str()));

	qreal radius = style.cornerRadius;
	m_rect->setRadius(radius);

	if (style.hasHatching)
	{
		QtDeviceScaledPixmap pattern((ResourcePaths::getGuiPath() + "graph_view/images/pattern.png").c_str());
		pattern.scaleToHeight(10);
		QPixmap pixmap = utility::colorizePixmap(pattern.pixmap(), style.color.hatching.c_str());

		if (!m_undefinedRect)
		{
			m_undefinedRect = new QtRoundedRectItem(this);
			setSize(getSize());
		}

		pen.setWidth(0);
		pen.setColor(Qt::transparent);

		m_undefinedRect->setPen(pen);
		m_undefinedRect->setBrush(pixmap);
		m_undefinedRect->setRadius(radius);
	}

	if (style.iconPath.size())
	{
		QtDeviceScaledPixmap pixmap(QString::fromStdString(style.iconPath));
		pixmap.scaleToHeight(style.iconSize);

		m_icon = new QGraphicsPixmapItem(utility::colorizePixmap(pixmap.pixmap(), style.color.icon.c_str()), this);
		m_icon->setPos(style.iconOffset.x, style.iconOffset.y);
	}

	QFont font(style.fontName.c_str());
	font.setPixelSize(style.fontSize);
	if (style.fontBold)
	{
		font.setWeight(QFont::Bold);
	}

	m_text->setFont(font);
	m_text->setBrush(QBrush(style.color.text.c_str()));
	m_text->setPos(style.iconOffset.x + style.iconSize + style.textOffset.x, style.textOffset.y);
}
