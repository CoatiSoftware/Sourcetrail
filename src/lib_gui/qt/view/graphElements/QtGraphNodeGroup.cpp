#include "qt/view/graphElements/QtGraphNodeGroup.h"

#include <QBrush>
#include <QGraphicsPolygonItem>
#include <QGraphicsSceneHoverEvent>
#include <QPainterPath>
#include <QPen>

#include "utility/messaging/type/MessageActivateNodes.h"
#include "utility/messaging/type/MessageFocusIn.h"
#include "utility/messaging/type/MessageFocusOut.h"
#include "utility/messaging/type/MessageGraphNodeBundleSplit.h"

#include "qt/graphics/QtRoundedRectItem.h"

QtGraphNodeGroup::QtGraphNodeGroup(
	Id tokenId, const std::wstring& name, GroupType type, bool interactive
)
	: m_tokenId(tokenId)
	, m_type(type)
	, m_interactive(interactive)
{
	if (interactive)
	{
		setAcceptHoverEvents(true);
	}

	setName(name);

	if (type == GroupType::FRAMELESS)
	{
		m_rect->hide();
		return;
	}

	if (!name.size())
	{
		return;
	}

	m_background = new QGraphicsPolygonItem(this);
	m_background->setZValue(-3.f);

	GraphViewStyle::NodeStyle style = GraphViewStyle::getStyleOfGroupNode(type, false);
	GraphViewStyle::NodeMargins margins = GraphViewStyle::getMarginsOfGroupNode(type, true);

	int width = style.textOffset.x * 2 + style.borderWidth + margins.charWidth * name.size();
	int height = margins.spacingA + margins.charHeight;
	int radius = style.cornerRadius;

	QPainterPath path;
	path.moveTo(width, 0);
	path.lineTo(radius, 0);
	path.arcTo(0, 0, 2 * radius, 2 * radius, 90, 90);
	path.lineTo(0, height);
	path.lineTo(width - radius, height);
	path.arcTo(width - 2 * radius, height - 2 * radius, 2 * radius, 2 * radius, 270, 90);
	path.closeSubpath();

	m_background->setPolygon(path.toFillPolygon());
}

QtGraphNodeGroup::~QtGraphNodeGroup()
{
}

bool QtGraphNodeGroup::isGroupNode() const
{
	return true;
}

Id QtGraphNodeGroup::getTokenId() const
{
	return m_tokenId;
}

void QtGraphNodeGroup::onClick()
{
	if (!m_interactive || !m_isHovering)
	{
		return;
	}

	if (m_type == GroupType::FILE || m_type == GroupType::NAMESPACE)
	{
		MessageActivateNodes(m_tokenId).dispatch();
	}
	else
	{
		MessageGraphNodeBundleSplit(m_tokenId).dispatch();
	}
}

void QtGraphNodeGroup::updateStyle()
{
	GraphViewStyle::NodeStyle style = GraphViewStyle::getStyleOfGroupNode(m_type, m_isHovering);

	if (m_background)
	{
		m_background->setBrush(QColor(style.color.border.c_str()));
		m_background->setPen(QPen(Qt::transparent));
	}

	setStyle(style);
}

void QtGraphNodeGroup::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	if (m_type == GroupType::FILE || m_type == GroupType::NAMESPACE)
	{
		MessageFocusOut({ m_tokenId }).dispatch();
	}
	else
	{
		focusOut();
	}
}

void QtGraphNodeGroup::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
	if (!m_background || m_background->contains(event->pos()))
	{
		if (!m_isHovering)
		{
			if (m_type == GroupType::FILE || m_type == GroupType::NAMESPACE)
			{
				MessageFocusIn({ m_tokenId }, TOOLTIP_ORIGIN_GRAPH).dispatch();
			}
			else
			{
				focusIn();
			}
		}
	}
	else if (m_isHovering)
	{
		hoverLeaveEvent(nullptr);
	}
}
