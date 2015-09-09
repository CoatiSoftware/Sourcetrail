#include "qt/view/graphElements/QtGraphNodeBundle.h"

#include <QBrush>
#include <QPen>

#include "utility/messaging/type/MessageGraphNodeBundleSplit.h"

#include "component/view/GraphViewStyle.h"
#include "qt/graphics/QtCountCircleItem.h"
#include "settings/ColorScheme.h"

QtGraphNodeBundle::QtGraphNodeBundle(Id tokenId, size_t nodeCount, std::string name)
	: QtGraphNode()
	, m_tokenId(tokenId)
{
	this->setName(name);

	m_circle = new QtCountCircleItem(this);
	m_circle->setNumber(nodeCount);

	this->setAcceptHoverEvents(true);
}

QtGraphNodeBundle::~QtGraphNodeBundle()
{
}

bool QtGraphNodeBundle::isBundleNode() const
{
	return true;
}

Id QtGraphNodeBundle::getTokenId() const
{
	return m_tokenId;
}

void QtGraphNodeBundle::onClick()
{
	MessageGraphNodeBundleSplit(m_tokenId).dispatch();
}

void QtGraphNodeBundle::updateStyle()
{
	GraphViewStyle::NodeStyle style = GraphViewStyle::getStyleOfBundleNode(m_isHovering);
	setStyle(style);

	m_circle->setPosition(Vec2f(m_rect->rect().right() - 3, m_rect->rect().top() + 3));

	ColorScheme* scheme = ColorScheme::getInstance().get();
	m_circle->setStyle(
		scheme->getColor("graph/background").c_str(),
		scheme->getColor("graph/text").c_str(),
		scheme->getColor("graph/text").c_str(),
		style.borderWidth
	);
}


void QtGraphNodeBundle::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
	focusIn();
}

void QtGraphNodeBundle::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	focusOut();
}
