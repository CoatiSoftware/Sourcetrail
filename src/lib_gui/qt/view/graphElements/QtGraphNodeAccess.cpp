#include "qt/view/graphElements/QtGraphNodeAccess.h"

#include <QBrush>
#include <QFontMetrics>
#include <QPen>

#include "utility/ResourcePaths.h"

#include "component/view/GraphViewStyle.h"
#include "qt/graphics/QtRoundedRectItem.h"
#include "qt/utility/QtDeviceScaledPixmap.h"
#include "qt/utility/utilityQt.h"

QtGraphNodeAccess::QtGraphNodeAccess(TokenComponentAccess::AccessType accessType)
	: QtGraphNode()
	, m_access(accessType)
	, m_accessIcon(nullptr)
	, m_accessIconSize(16)
{
	std::string accessString = TokenComponentAccess::getAccessString(accessType);
	this->setName(accessString);
	m_text->hide();

	std::string fileName;
	switch (accessType)
	{
	case TokenComponentAccess::ACCESS_PUBLIC:
		fileName = "public";
		break;
	case TokenComponentAccess::ACCESS_PROTECTED:
		fileName = "protected";
		break;
	case TokenComponentAccess::ACCESS_PRIVATE:
		fileName = "private";
		break;
	case TokenComponentAccess::ACCESS_TEMPLATE:
		fileName = "template";
		break;
	default:
		break;
	}

	if (fileName.size() > 0)
	{
		QtDeviceScaledPixmap pixmap(
			QString::fromStdString(ResourcePaths::getGuiPath() + "graph_view/images/" + fileName + ".png"));
		pixmap.scaleToHeight(m_accessIconSize);

		m_accessIcon = new QGraphicsPixmapItem(pixmap.pixmap(), this);
		m_accessIcon->setTransformationMode(Qt::SmoothTransformation);
	}
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

	if (m_accessIcon)
	{
		m_text->setPos(style.textOffset.x + m_accessIconSize + 3, style.textOffset.y + m_accessIconSize - style.fontSize);
		m_accessIcon->setPos(style.textOffset.x, style.textOffset.y);

		m_accessIcon->setPixmap(utility::colorizePixmap(m_accessIcon->pixmap(), style.color.icon.c_str()));
	}
	else
	{
		m_text->setPos(style.textOffset.x, style.textOffset.y + m_accessIconSize + 2 - style.fontSize);
	}
}

void QtGraphNodeAccess::hideLabel()
{
	m_text->hide();
}
