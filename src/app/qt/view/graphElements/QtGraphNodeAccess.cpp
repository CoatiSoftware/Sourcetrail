#include "qt/view/graphElements/QtGraphNodeAccess.h"

#include <QBrush>
#include <QFontMetrics>
#include <QPen>

#include "component/view/GraphViewStyle.h"
#include "qt/graphics/QtRoundedRectItem.h"
#include "qt/utility/QtDeviceScaledPixmap.h"
#include "qt/utility/utilityQt.h"

QtGraphNodeAccess::QtGraphNodeAccess(TokenComponentAccess::AccessType accessType)
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
}

QtGraphNodeAccess::~QtGraphNodeAccess()
{
}

TokenComponentAccess::AccessType QtGraphNodeAccess::getAccessType() const
{
	return m_access;
}

bool QtGraphNodeAccess::isAccessNode() const
{
	return true;
}

void QtGraphNodeAccess::addSubNode(const std::shared_ptr<QtGraphNode>& node)
{
	QtGraphNode::addSubNode(node);
	m_text->show();
}

void QtGraphNodeAccess::updateStyle()
{
	GraphViewStyle::NodeStyle style = GraphViewStyle::getStyleOfAccessNode();
	setStyle(style);

	QFont font = m_text->font();
	font.setCapitalization(QFont::AllUppercase);
	m_text->setFont(font);

	m_text->setPos(style.textOffset.x + m_accessIconSize + 3, style.textOffset.x + m_accessIconSize + 2 - style.fontSize);
	m_accessIcon->setPos(style.textOffset.x, style.textOffset.y);

	m_accessIcon->setPixmap(utility::colorizePixmap(m_accessIcon->pixmap(), style.iconColor.c_str()));
}

void QtGraphNodeAccess::hideLabel()
{
	m_text->hide();
}
