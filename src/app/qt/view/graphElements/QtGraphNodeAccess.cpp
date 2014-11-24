#include "qt/view/graphElements/QtGraphNodeAccess.h"

#include <QBrush>
#include <QFontMetrics>
#include <QPen>

#include "utility/messaging/type/MessageGraphNodeExpand.h"

#include "qt/graphics/QtGraphicsRoundedRectItem.h"
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
			font.setFamily("Myriad Pro");
			font.setWeight(QFont::Normal);
			font.setPixelSize(9);

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
	Vec2i padding(10, 10);

	QFont font;
	font.setFamily("Myriad Pro");
	font.setWeight(QFont::Bold);
	font.setPixelSize(11);
	font.setCapitalization(QFont::AllUppercase);
	m_text->setFont(font);
	m_text->setPos(padding.x + m_accessIconSize + 3, padding.y + m_accessIconSize / 2 - 1);

	m_rect->setPen(QPen(Qt::transparent));
	m_rect->setBrush(QBrush(Qt::white));
	m_rect->setRadius(12.0f);

	std::string accessString = TokenComponentAccess::getAccessString(accessType);
	this->setName(accessString);
	m_text->hide();

	QtDeviceScaledPixmap pixmap(QString::fromStdString("data/gui/graph_view/images/" + accessString + ".png"));
	pixmap.scaleToHeight(m_accessIconSize);

	m_accessIcon = new QGraphicsPixmapItem(pixmap.pixmap(), this);
	m_accessIcon->setPos(padding.x, padding.y);

	m_accessToggle = new QtAccessToggle(expanded, invisibleSubNodeCount, this);
}

QtGraphNodeAccess::~QtGraphNodeAccess()
{
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
