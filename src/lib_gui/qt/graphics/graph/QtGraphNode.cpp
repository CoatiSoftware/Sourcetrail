#include "QtGraphNode.h"

#include <QBrush>
#include <QCursor>
#include <QFont>
#include <QFontMetrics>
#include <QGraphicsSceneEvent>
#include <QPen>

#include "MessageCodeShowDefinition.h"
#include "MessageGraphNodeExpand.h"
#include "MessageGraphNodeHide.h"
#include "MessageGraphNodeMove.h"
#include "QtDeviceScaledPixmap.h"
#include "QtGraphEdge.h"
#include "QtGraphNodeComponent.h"
#include "QtGraphNodeExpandToggle.h"
#include "QtRoundedRectItem.h"
#include "ResourcePaths.h"
#include "utilityQt.h"
#include "utilityString.h"

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

QtGraphNode::QtGraphNode()
{
	this->setPen(QPen(Qt::transparent));
	this->setCursor(Qt::PointingHandCursor);

	m_text = new QGraphicsSimpleTextItem(this);
	m_rect = new QtRoundedRectItem(this);
	m_undefinedRect = new QtRoundedRectItem(this);
	m_undefinedRect->hide();
}

QtGraphNode::~QtGraphNode() {}

QtGraphNode* QtGraphNode::getParent() const
{
	return m_parentNode;
}

QtGraphNode* QtGraphNode::getLastParent(bool noGroups) const
{
	QtGraphNode* node = const_cast<QtGraphNode*>(this);
	while (true)
	{
		QtGraphNode* parent = dynamic_cast<QtGraphNode*>(node->parentItem());
		if (!parent || (noGroups && parent->isGroupNode()))
		{
			break;
		}
		node = parent;
	}
	return node;
}

QtGraphNode* QtGraphNode::getLastNonGroupParent() const
{
	return getLastParent(true);
}

void QtGraphNode::setParent(QtGraphNode* parentNode)
{
	m_parentNode = parentNode;

	if (m_parentNode != nullptr)
	{
		QGraphicsRectItem::setParentItem(m_parentNode);
	}
}

const std::list<QtGraphNode*>& QtGraphNode::getSubNodes() const
{
	return m_subNodes;
}

Vec2i QtGraphNode::getPosition() const
{
	return Vec2i(static_cast<int>(this->scenePos().x()), static_cast<int>(this->scenePos().y()));
}

bool QtGraphNode::setPosition(const Vec2i& position)
{
	Vec2i currentPosition = getPosition();
	Vec2i offset = position - currentPosition;

	if (offset.x != 0 || offset.y != 0)
	{
		this->moveBy(offset.x, offset.y);
		setColumnSize(Vec2i());
		notifyEdgesAfterMove();
		return true;
	}

	return false;
}

const Vec2i& QtGraphNode::getSize() const
{
	return m_size;
}

void QtGraphNode::setSize(const Vec2i& size)
{
	m_size = size;

	this->setRect(0, 0, size.x, size.y);
	m_rect->setRect(0, 0, size.x, size.y);
	m_undefinedRect->setRect(1, 1, size.x - 2, size.y - 2);
}

const Vec2i& QtGraphNode::getColumnSize() const
{
	return m_columnSize;
}

void QtGraphNode::setColumnSize(const Vec2i& size)
{
	m_columnSize = size;
}

QSize QtGraphNode::size() const
{
	return QSize(m_size.x, m_size.y);
}

void QtGraphNode::setSize(QSize size)
{
	setSize(Vec2i(size.width(), size.height()));
}

Vec4i QtGraphNode::getBoundingRect() const
{
	Vec2i pos = getPosition();
	Vec2i size = getSize();
	return Vec4i(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
}

void QtGraphNode::addOutEdge(QtGraphEdge* edge)
{
	m_outEdges.push_back(edge);
}

void QtGraphNode::addInEdge(QtGraphEdge* edge)
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
}

void QtGraphNode::setMultipleActive(bool multipleActive)
{
	m_multipleActive = multipleActive;
}

bool QtGraphNode::hasActiveChild() const
{
	if (m_isActive)
	{
		return true;
	}

	for (auto subNode: m_subNodes)
	{
		if (subNode->hasActiveChild())
		{
			return true;
		}
	}

	return false;
}

std::wstring QtGraphNode::getName() const
{
	return m_text->text().toStdWString();
}

void QtGraphNode::setName(const std::wstring& name)
{
	m_text->setText(QString::fromStdWString(name));
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

	forEachEdge([](QtGraphEdge* edge) {
		if (edge->isTrailEdge())
		{
			edge->focusIn();
		}
	});

	updateStyle();
}

void QtGraphNode::focusOut()
{
	m_isHovering = false;

	forEachEdge([](QtGraphEdge* edge) {
		if (edge->isTrailEdge())
		{
			edge->focusOut();
		}
	});

	updateStyle();
}

void QtGraphNode::showNodeRecursive()
{
	blendIn();
	showNode();

	for (auto subNode: m_subNodes)
	{
		subNode->showNodeRecursive();
	}
}

void QtGraphNode::matchNameRecursive(const std::wstring& query, std::vector<QtGraphNode*>* matchedNodes)
{
	matchName(query, matchedNodes);

	for (auto subNode: m_subNodes)
	{
		subNode->matchNameRecursive(query, matchedNodes);
	}
}

void QtGraphNode::removeNameMatch()
{
	if (m_matchLength)
	{
		m_matchRect->hide();
		m_matchText->hide();

		m_matchPos = 0;
		m_matchLength = 0;

		updateStyle();
	}
}

void QtGraphNode::setActiveMatch(bool active)
{
	m_isActiveMatch = active;
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

bool QtGraphNode::isQualifierNode() const
{
	return false;
}

bool QtGraphNode::isTextNode() const
{
	return false;
}

bool QtGraphNode::isGroupNode() const
{
	return false;
}

Id QtGraphNode::getTokenId() const
{
	return 0;
}

void QtGraphNode::addSubNode(QtGraphNode* node)
{
	m_subNodes.push_back(node);

	// push parent nodes to back so all edges going to the active subnode are visible
	if (node->getIsActive())
	{
		QtGraphNode* parent = this;
		while (parent && !parent->isGroupNode())
		{
			parent->setZValue(-10.0f);
			parent->m_text->setZValue(-9.0f);
			parent->m_rect->setZValue(-10.0f);
			parent->m_undefinedRect->setZValue(-10.0f);

			parent = parent->getParent();
		}
	}
}

void QtGraphNode::moved(const Vec2i& oldPosition)
{
	setPosition(GraphViewStyle::alignOnRaster(getPosition()));

	if (isDataNode() || isGroupNode() || isBundleNode())
	{
		MessageGraphNodeMove(getTokenId(), getPosition() - oldPosition).dispatch();
	}
}

void QtGraphNode::onClick() {}

void QtGraphNode::onMiddleClick() {}

void QtGraphNode::onHide()
{
	Id tokenId = getTokenId();

	if (tokenId)
	{
		MessageGraphNodeHide(tokenId).dispatch();
	}
	else if (getParent())
	{
		getParent()->onHide();
	}
}

void QtGraphNode::onCollapseExpand()
{
	for (auto subNode: getSubNodes())
	{
		if (subNode->isExpandToggleNode())
		{
			MessageGraphNodeExpand(
				getTokenId(), !dynamic_cast<QtGraphNodeExpandToggle*>(subNode)->isExpanded())
				.dispatch();
			return;
		}
	}

	if (getParent())
	{
		getParent()->onCollapseExpand();
	}
}

void QtGraphNode::onShowDefinition(bool inIDE)
{
	Id tokenId = getTokenId();

	if (tokenId)
	{
		MessageCodeShowDefinition(tokenId, inIDE).dispatch();
	}
	else if (getParent())
	{
		getParent()->onShowDefinition(inIDE);
	}
}

void QtGraphNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	event->ignore();

	for (std::shared_ptr<QtGraphNodeComponent> component: m_components)
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

	for (std::shared_ptr<QtGraphNodeComponent> component: m_components)
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

	for (std::shared_ptr<QtGraphNodeComponent> component: m_components)
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

void QtGraphNode::forEachEdge(std::function<void(QtGraphEdge*)> func)
{
	for (QtGraphEdge* edge: m_outEdges)
	{
		func(edge);
	}

	for (QtGraphEdge* edge: m_inEdges)
	{
		func(edge);
	}
}

void QtGraphNode::notifyEdgesAfterMove()
{
	forEachEdge([](QtGraphEdge* edge) {
		edge->clearPath();
		edge->updateLine();
	});

	for (QtGraphNode* node: m_subNodes)
	{
		node->notifyEdgesAfterMove();
	}
}

void QtGraphNode::matchName(const std::wstring& query, std::vector<QtGraphNode*>* matchedNodes)
{
	m_isActiveMatch = false;
	const std::wstring name = getName();
	size_t pos = utility::toLowerCase(name).find(query);

	if (pos != std::string::npos)
	{
		if (!m_matchText)
		{
			m_matchRect = new QtRoundedRectItem(this);
			m_matchText = new QGraphicsSimpleTextItem(this);
		}

		m_matchRect->show();
		m_matchText->show();

		std::wstring matchName(name.length(), L' ');
		matchName.replace(pos, query.length(), name.substr(pos, query.length()));
		m_matchText->setText(QString::fromStdWString(matchName));

		matchedNodes->push_back(this);

		m_matchPos = pos;
		m_matchLength = query.length();

		updateStyle();
	}
	else if (m_matchLength)
	{
		removeNameMatch();
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
		QtDeviceScaledPixmap pattern(QString::fromStdWString(
			ResourcePaths::getGuiPath().concatenate(L"graph_view/images/pattern.png").wstr()));
		pattern.scaleToHeight(12);
		QPixmap pixmap = utility::colorizePixmap(pattern.pixmap(), style.color.hatching.c_str());

		pen.setWidth(0);
		pen.setColor(Qt::transparent);

		m_undefinedRect->setPen(pen);
		m_undefinedRect->setBrush(pixmap);
		m_undefinedRect->setRadius(radius);
		m_undefinedRect->show();
	}

	if (!m_icon && !style.iconPath.empty())
	{
		QtDeviceScaledPixmap pixmap(QString::fromStdWString(style.iconPath.wstr()));
		pixmap.scaleToHeight(static_cast<int>(style.iconSize));

		m_icon = new QGraphicsPixmapItem(
			utility::colorizePixmap(pixmap.pixmap(), style.color.icon.c_str()), this);
		m_icon->setTransformationMode(Qt::SmoothTransformation);
		m_icon->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
		m_icon->setPos(style.iconOffset.x, style.iconOffset.y);
	}

	QFont font(style.fontName.c_str());
	font.setPixelSize(static_cast<int>(style.fontSize));
	if (style.fontBold)
	{
		font.setWeight(QFont::Bold);
	}

	m_text->setFont(font);
	m_text->setBrush(QBrush(style.color.text.c_str()));
	m_text->setPos(
		static_cast<qreal>(style.iconOffset.x + style.iconSize + style.textOffset.x),
		static_cast<qreal>(style.textOffset.y));

	if (m_matchLength)
	{
		GraphViewStyle::NodeColor color = GraphViewStyle::getScreenMatchColor(m_isActiveMatch);

		m_matchText->setFont(font);
		m_matchText->setBrush(QBrush(color.text.c_str()));
		m_matchText->setPos(
			static_cast<qreal>(style.iconOffset.x + style.iconSize + style.textOffset.x),
			static_cast<qreal>(style.textOffset.y));

		const float charWidth =
			QFontMetrics(font).width(QStringLiteral("QtGraphNode::QtGraphNode::QtGraphNode")) / 37.0f;
		const float charHeight = static_cast<float>(QFontMetrics(font).height());
		m_matchRect->setRect(
			static_cast<qreal>(
				style.iconOffset.x + style.iconSize + style.textOffset.x + m_matchPos * charWidth),
			static_cast<qreal>(style.textOffset.y),
			static_cast<qreal>(m_matchLength * charWidth),
			static_cast<qreal>(charHeight));
		m_matchRect->setPen(QPen(color.border.c_str()));
		m_matchRect->setBrush(QBrush(color.fill.c_str()));
		m_matchRect->setRadius(3);
	}
}
