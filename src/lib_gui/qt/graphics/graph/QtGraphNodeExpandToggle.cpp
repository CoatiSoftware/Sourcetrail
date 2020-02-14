#include "QtGraphNodeExpandToggle.h"

#include <QFontMetrics>

#include "QtDeviceScaledPixmap.h"
#include "QtRoundedRectItem.h"
#include "utilityQt.h"

#include "MessageGraphNodeExpand.h"
#include "ResourcePaths.h"
#include "logging.h"

QtGraphNodeExpandToggle::QtGraphNodeExpandToggle(bool expanded, int invisibleSubNodeCount)
	: m_invisibleSubNodeCount(invisibleSubNodeCount), m_expanded(expanded)
{
	if (!expanded && !invisibleSubNodeCount)
	{
		LOG_ERROR("ExpandToggle shouldn't be visible");
		return;
	}

	const int iconHeight = 4;
	m_icon = new QGraphicsPixmapItem(this);
	m_icon->setTransformationMode(Qt::SmoothTransformation);
	m_icon->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);

	QtDeviceScaledPixmap pixmap(QString::fromStdWString(
		ResourcePaths::getGuiPath().concatenate(L"graph_view/images/arrow.png").wstr()));
	pixmap.scaleToHeight(iconHeight);

	if (invisibleSubNodeCount)
	{
		QString numberStr = QString::number(invisibleSubNodeCount);
		m_text->setText(numberStr);
	}
	else
	{
		pixmap.mirror();
	}

	m_icon->setPixmap(pixmap.pixmap());
}

QtGraphNodeExpandToggle::~QtGraphNodeExpandToggle() {}

bool QtGraphNodeExpandToggle::isExpandToggleNode() const
{
	return true;
}

void QtGraphNodeExpandToggle::onClick()
{
	QtGraphNode* parent = getParent();

	if (parent && parent->getTokenId())
	{
		MessageGraphNodeExpand(parent->getTokenId(), !m_expanded).dispatch();
	}
}

void QtGraphNodeExpandToggle::updateStyle()
{
	if (!m_icon)
	{
		return;
	}

	GraphViewStyle::NodeStyle style = GraphViewStyle::getStyleOfExpandToggleNode();
	setStyle(style);

	float textX = static_cast<float>(
		(m_rect->rect().width() / 2) - (QFontMetrics(m_text->font()).width(m_text->text()) / 2));
	const float textY = static_cast<float>(
		m_rect->rect().height() / 2 - QFontMetrics(m_text->font()).height() / 1.8f);

	// move the text to the nearest integer x pos, instead of the next lower int pos
	// improves results on windows systems
	if (textX - (float)((int)textX) >= 0.5f)
	{
		textX += 0.5f;
	}

	m_text->setPos(textX, textY);

	m_icon->setPos(
		(m_rect->rect().width() - m_icon->pixmap().width() / QtDeviceScaledPixmap::devicePixelRatio()) /
			2,
		(m_invisibleSubNodeCount == 0 ? m_rect->rect().height() / 2 - 2
									  : m_rect->rect().height() - 7));

	m_icon->setPixmap(utility::colorizePixmap(m_icon->pixmap(), style.color.icon.c_str()));
}

bool QtGraphNodeExpandToggle::isExpanded() const
{
	return m_expanded;
}
