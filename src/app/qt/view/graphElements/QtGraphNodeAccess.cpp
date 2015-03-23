#include "qt/view/graphElements/QtGraphNodeAccess.h"

#include <QBrush>
#include <QFontMetrics>
#include <QPen>

#include "utility/messaging/type/MessageGraphNodeExpand.h"

#include "component/view/GraphViewStyle.h"
#include "qt/graphics/QtRoundedRectItem.h"
#include "qt/utility/QtDeviceScaledPixmap.h"

QtGraphNodeAccess::QtAccessToggle::QtAccessToggle(bool expanded, int invisibleSubNodeCount, QGraphicsItem* parent)
	: QGraphicsRectItem(parent)
{
	const int iconHeight = 4;
	m_icon = new QGraphicsPixmapItem(this);

	if (!expanded && !invisibleSubNodeCount)
	{
		this->hide();
		return;
	}
	else
	{
		QtDeviceScaledPixmap pixmap("data/gui/graph_view/images/arrow.png");
		pixmap.scaleToHeight(iconHeight);

		if (invisibleSubNodeCount)
		{
			QFont font;
			font.setFamily(GraphViewStyle::getFontNameOfNumber().c_str());
			font.setPixelSize(GraphViewStyle::getFontSizeOfNumber());
			font.setWeight(QFont::Normal);

			m_number = new QGraphicsSimpleTextItem(this);
			m_number->setFont(font);

			QString numberStr = QString::number(invisibleSubNodeCount);
			m_number->setText(numberStr);
			m_number->setPos(
				-QFontMetrics(m_number->font()).width(numberStr) / 2,
				-iconHeight - QFontMetrics(m_number->font()).height()
			);
		}
		else
		{
			pixmap.mirror();
		}

		m_icon->setPixmap(pixmap.pixmap());
		m_icon->setPos(-pixmap.width() / 2, -iconHeight);
	}
}

QtGraphNodeAccess::QtAccessToggle::~QtAccessToggle()
{
}


QtGraphNodeAccess::QtGraphNodeAccess(
	TokenComponentAccess::AccessType accessType, bool expanded, int invisibleSubNodeCount
)
	: QtGraphNode()
	, m_access(accessType)
	, m_accessIconSize(20)
{
	std::string accessString = TokenComponentAccess::getAccessString(accessType);
	this->setName(accessString);
	m_text->hide();

	QtDeviceScaledPixmap pixmap(QString::fromStdString("data/gui/graph_view/images/" + accessString + ".png"));
	pixmap.scaleToHeight(m_accessIconSize);

	m_accessIcon = new QGraphicsPixmapItem(pixmap.pixmap(), this);
	m_accessToggle = new QtAccessToggle(expanded, invisibleSubNodeCount, this);
}

QtGraphNodeAccess::~QtGraphNodeAccess()
{
}

bool QtGraphNodeAccess::isAccessNode() const
{
	return true;
}

TokenComponentAccess::AccessType QtGraphNodeAccess::getAccessType() const
{
	return m_access;
}

void QtGraphNodeAccess::setSize(const Vec2i& size)
{
	QtGraphNode::setSize(size);

	m_accessToggle->setPos(m_rect->rect().width() / 2, m_rect->rect().height() - 5);
}

void QtGraphNodeAccess::addSubNode(const std::shared_ptr<QtGraphNode>& node)
{
	QtGraphNode::addSubNode(node);
	m_text->show();
}

void QtGraphNodeAccess::onClick()
{
	QtGraphNode* parent = getParent();

	if (m_accessToggle->isVisible() && parent && parent->getData())
	{
		MessageGraphNodeExpand(parent->getData()->getId(), m_access).dispatch();
	}
}

void QtGraphNodeAccess::updateStyle()
{
	GraphViewStyle::NodeStyle style = GraphViewStyle::getStyleOfAccessNode();
	setStyle(style);

	QFont font = m_text->font();
	font.setCapitalization(QFont::AllUppercase);
	m_text->setFont(font);

	m_text->setPos(style.textOffset.x + m_accessIconSize + 3, style.textOffset.y + m_accessIconSize / 2 - 1);
	m_accessIcon->setPos(style.textOffset.x, style.textOffset.y);
}

void QtGraphNodeAccess::hideLabel()
{
	m_text->hide();
}
