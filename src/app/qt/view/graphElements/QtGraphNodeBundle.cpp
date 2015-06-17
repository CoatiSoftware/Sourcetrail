#include "qt/view/graphElements/QtGraphNodeBundle.h"

#include <sstream>

#include <QBrush>
#include <QPen>

#include "utility/messaging/type/MessageGraphNodeBundleSplit.h"

#include "component/view/GraphViewStyle.h"
#include "qt/graphics/QtRoundedRectItem.h"

QtGraphNodeBundle::QtGraphNodeBundle(Id tokenId, size_t nodeCount, std::string name)
	: QtGraphNode()
	, m_tokenId(tokenId)
{
	std::stringstream ss;
	ss << nodeCount << " " << name;
	this->setName(ss.str());

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

	if (!m_undefinedRect)
	{
		m_undefinedRect = new QtRoundedRectItem(this);
		setSize(getSize());
		m_undefinedRect->moveBy(7, 7);
	}

	m_undefinedRect->setPen(m_rect->pen());
	m_undefinedRect->setBrush(m_rect->brush());
	m_undefinedRect->setRadius(style.cornerRadius);
	m_undefinedRect->setZValue(-1);
	m_rect->setZValue(0);
}


void QtGraphNodeBundle::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
	focusIn();
}

void QtGraphNodeBundle::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	focusOut();
}
