#include "QtGraphNodeBundle.h"

#include <QBrush>
#include <QPen>

#include "MessageGraphNodeBundleSplit.h"

#include "GraphViewStyle.h"
#include "QtCountCircleItem.h"

QtGraphNodeBundle::QtGraphNodeBundle(Id tokenId, size_t nodeCount, NodeType type, const std::wstring& name)
	: QtGraphNode(), m_tokenId(tokenId), m_type(type)
{
	this->setName(name);

	m_circle = new QtCountCircleItem(this);
	m_circle->setNumber(nodeCount);

	this->setAcceptHoverEvents(true);

	this->setToolTip(QStringLiteral("bundle"));
}

QtGraphNodeBundle::~QtGraphNodeBundle() {}

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
	MessageGraphNodeBundleSplit(
		m_tokenId,
		(!m_type.isUnknownSymbol() || getName() == L"Symbols") &&
			getName() != L"Anonymous Namespaces",	 // TODO: move to language package
		!m_type.isUnknownSymbol() || getName() == L"Symbols")
		.dispatch();
}

void QtGraphNodeBundle::updateStyle()
{
	GraphViewStyle::NodeStyle style;
	if (!m_type.isUnknownSymbol())
	{
		style = GraphViewStyle::getStyleForNodeType(m_type, true, false, m_isHovering, false, false);
	}
	else
	{
		style = GraphViewStyle::getStyleOfBundleNode(m_isHovering);
	}
	setStyle(style);

	Vec2f pos(
		static_cast<float>(m_rect->rect().right()), static_cast<float>(m_rect->rect().top() - 2));
	if (m_type.getNodeStyle() == NodeType::STYLE_BIG_NODE)
	{
		pos += Vec2f(-2, 2);
	}
	m_circle->setPosition(pos);

	GraphViewStyle::NodeStyle accessStyle = GraphViewStyle::getStyleOfCountCircle();
	m_circle->setStyle(
		accessStyle.color.fill.c_str(),
		accessStyle.color.text.c_str(),
		accessStyle.color.border.c_str(),
		style.borderWidth);
}


void QtGraphNodeBundle::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
	focusIn();
}

void QtGraphNodeBundle::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	focusOut();
}
