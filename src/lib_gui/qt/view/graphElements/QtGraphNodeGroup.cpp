#include "qt/view/graphElements/QtGraphNodeGroup.h"

#include <QBrush>
#include <QGraphicsRectItem>
#include <QPen>

#include "utility/messaging/type/MessageGraphNodeBundleSplit.h"

#include "qt/graphics/QtRoundedRectItem.h"

QtGraphNodeGroup::QtGraphNodeGroup(Id tokenId, const std::wstring& name, NodeType::GroupType type)
	: m_tokenId(tokenId)
	, m_type(type)
{
	setAcceptHoverEvents(true);

	setName(name);
	setZValue(-10.0f);
	m_rect->setZValue(-10.0f);

	if (type == NodeType::GROUP_FRAMELESS)
	{
		m_rect->hide();
		return;
	}

	if (!name.size())
	{
		return;
	}

	m_background = new QtRoundedRectItem(this);
	m_backgroundTopRight = new QGraphicsRectItem(this);
	m_backgroundBottomLeft = new QGraphicsRectItem(this);

	m_background->setZValue(-10.0f);
	m_backgroundTopRight->setZValue(-10.0f);
	m_backgroundBottomLeft->setZValue(-10.0f);

	GraphViewStyle::NodeStyle style = GraphViewStyle::getStyleOfGroupNode(type, false);
	GraphViewStyle::NodeMargins margins = GraphViewStyle::getMarginsOfGroupNode(type, true);

	int width = style.textOffset.x * 2 + style.borderWidth + margins.charWidth * name.size();
	int height = margins.top * 2 + margins.charHeight;

	m_background->setRadius(style.cornerRadius);
	m_background->setRect(0, 0, width, height);

	m_backgroundTopRight->setRect(width - style.cornerRadius, 0, style.cornerRadius, style.cornerRadius);
	m_backgroundBottomLeft->setRect(0, height - style.cornerRadius, style.cornerRadius, style.cornerRadius);
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
	MessageGraphNodeBundleSplit(m_tokenId).dispatch();
}

void QtGraphNodeGroup::updateStyle()
{
	GraphViewStyle::NodeStyle style = GraphViewStyle::getStyleOfGroupNode(m_type, false);

	if (m_background)
	{
		m_background->setBrush(QColor(style.color.border.c_str()));
		m_background->setPen(QPen(Qt::transparent));

		m_backgroundTopRight->setBrush(QColor(style.color.border.c_str()));
		m_backgroundTopRight->setPen(QPen(Qt::transparent));

		m_backgroundBottomLeft->setBrush(QColor(style.color.border.c_str()));
		m_backgroundBottomLeft->setPen(QPen(Qt::transparent));
	}

	setStyle(style);
}
