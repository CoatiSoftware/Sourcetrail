#include "qt/view/graphElements/QtGraphNodeAccess.h"

#include <QBrush>
#include <QFontMetrics>
#include <QPen>

#include "qt/graphics/QtGraphicsRoundedRectItem.h"
#include "qt/utility/QtDeviceScaledPixmap.h"

QtGraphNodeAccess::QtAccessArrow::QtAccessArrow(QGraphicsItem* parent)
	: QGraphicsRectItem(parent)
{
	m_icon = new QGraphicsPixmapItem(this);

	QFont font;
	font.setFamily("Myriad Pro");
	font.setWeight(QFont::Normal);
	font.setPointSize(9);

	m_number = new QGraphicsSimpleTextItem(this);
	m_number->setFont(font);
}

QtGraphNodeAccess::QtAccessArrow::~QtAccessArrow()
{
}

int QtGraphNodeAccess::QtAccessArrow::update(bool visible, int number)
{
	const int iconHeight = 4;

	int spacing = 0;

	if (!visible)
	{
		m_icon->hide();
		m_number->hide();
		return spacing;
	}

	m_icon->show();

	QtDeviceScaledPixmap pixmap("data/gui/graph_view/images/arrow.png");
	pixmap.scaleToHeight(iconHeight);

	if (number)
	{
		m_number->show();
		spacing = 13;

		QString numberStr = QString::number(number);
		m_number->setText(numberStr);
		m_number->setPos(
			-QFontMetrics(m_number->font()).width(numberStr) / 2,
			-iconHeight - QFontMetrics(m_number->font()).height()
		);
	}
	else
	{
		m_number->hide();
		spacing = 5;

		pixmap.mirror();
	}

	m_icon->setPixmap(pixmap.pixmap());
	m_icon->setPos(-pixmap.width() / 2, -iconHeight);

	return spacing;
}


QtGraphNodeAccess::QtGraphNodeAccess(TokenComponentAccess::AccessType accessType)
	: QtGraphNode()
	, m_contentHidden(false)
	, m_accessIconSize(20)
{
	m_padding = Vec4i(10, 10, 10, 10);
	m_spacing = 8;

	QFont font;
	font.setFamily("Myriad Pro");
	font.setWeight(QFont::Bold);
	font.setPointSize(11);
	font.setCapitalization(QFont::AllUppercase);
	m_text->setFont(font);
	m_text->setPos(m_padding.x + m_accessIconSize + 3, m_padding.y + m_accessIconSize / 2 - 1);

	m_rect->setPen(QPen(Qt::transparent));
	m_rect->setBrush(QBrush(Qt::white));
	m_rect->setRadius(12.0f);

	std::string accessString = TokenComponentAccess::getAccessString(accessType);
	this->setName(accessString);

	QtDeviceScaledPixmap pixmap(QString::fromStdString("data/gui/graph_view/images/" + accessString + ".png"));
	pixmap.scaleToHeight(m_accessIconSize);

	m_accessIcon = new QGraphicsPixmapItem(pixmap.pixmap(), this);
	m_accessIcon->setPos(m_padding.x, m_padding.y);

	m_arrow = new QtAccessArrow(this);
}

QtGraphNodeAccess::~QtGraphNodeAccess()
{
}

void QtGraphNodeAccess::hideContent()
{
	m_contentHidden = true;

	QtGraphNode::hideContent();
}

void QtGraphNodeAccess::showContent()
{
	for (std::shared_ptr<QtGraphNode> node : m_subNodes)
	{
		node->show();
	}

	m_contentHidden = false;
}

void QtGraphNodeAccess::onClick()
{
	if (m_contentHidden)
	{
		showContent();
	}
	else
	{
		hideContent();
	}

	onSizeChanged();
}

void QtGraphNodeAccess::rebuildLayout()
{
	bool allActive = true;
	int visibleSubNodes = 0;
	for (const std::shared_ptr<QtGraphNode>& node : m_subNodes)
	{
		if (!node->getEdgeAndActiveCountRecursive())
		{
			allActive = false;
		}

		if (node->isVisible())
		{
			visibleSubNodes++;
		}
	}

	int width = m_padding.x + m_accessIconSize;
	int height = m_padding.y + m_accessIconSize;

	int arrowSpace = m_arrow->update(!allActive, m_subNodes.size() - visibleSubNodes);

	if (visibleSubNodes)
	{
		m_text->show();
		width += QFontMetrics(m_text->font()).width(m_text->text()) + 3;
	}
	else
	{
		m_text->hide();
		arrowSpace -= 3;
	}

	m_padding.w = 10 + arrowSpace;

	width += m_padding.z;
	height += m_padding.w;

	m_baseSize.x = width;
	m_baseSize.y = height;

	QtGraphNode::rebuildLayout();

	m_arrow->setPos(m_currentSize.x / 2, m_currentSize.y - 5);
}
