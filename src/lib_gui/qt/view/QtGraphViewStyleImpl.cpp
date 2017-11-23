#include "qt/view/QtGraphViewStyleImpl.h"

#include <QFontMetrics>

#include "qt/view/graphElements/QtGraphNode.h"
#include "utility/utilityApp.h"

QtGraphViewStyleImpl::~QtGraphViewStyleImpl()
{
}

float QtGraphViewStyleImpl::getCharWidth(NodeType::StyleType type)
{
	return QFontMetrics(QtGraphNode::getFontForStyleType(type)).width("QtGraphNode::QtGraphNode::QtGraphNode") / 37.0f;
}

float QtGraphViewStyleImpl::getCharHeight(NodeType::StyleType type)
{
	return QFontMetrics(QtGraphNode::getFontForStyleType(type)).height();
}

float QtGraphViewStyleImpl::getGraphViewZoomDifferenceForPlatform()
{
	if (utility::getOsType() == OS_MAC)
	{
		return 1;
	}

	return 1.25;
}
