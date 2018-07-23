#include "qt/view/QtGraphViewStyleImpl.h"

#include <QFont>
#include <QFontMetrics>

#include "component/view/GraphViewStyle.h"
#include "utility/utilityApp.h"

float QtGraphViewStyleImpl::getCharWidth(const std::string& fontName, size_t fontSize)
{
	return QFontMetrics(getFontForStyleType(fontName, fontSize)).width("QtGraphNode::QtGraphNode::QtGraphNode") / 37.0f;
}

float QtGraphViewStyleImpl::getCharHeight(const std::string& fontName, size_t fontSize)
{
	return QFontMetrics(getFontForStyleType(fontName, fontSize)).height();
}

float QtGraphViewStyleImpl::getGraphViewZoomDifferenceForPlatform()
{
	if (utility::getOsType() == OS_MAC)
	{
		return 1;
	}

	return 1.25;
}

QFont QtGraphViewStyleImpl::getFontForStyleType(const std::string& fontName, size_t fontSize) const
{
	QFont font(fontName.c_str());
	font.setPixelSize(fontSize);
	return font;
}
