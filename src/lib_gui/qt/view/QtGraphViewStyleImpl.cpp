#include "qt/view/QtGraphViewStyleImpl.h"

#include <QFontMetrics>

#include "qt/view/graphElements/QtGraphNode.h"
#include "utility/utilityApp.h"

QtGraphViewStyleImpl::~QtGraphViewStyleImpl()
{
}

float QtGraphViewStyleImpl::getCharWidthForNodeType(NodeType type)
{
	return QFontMetrics(QtGraphNode::getFontForNodeType(type)).width("QtGraphNode::QtGraphNode::QtGraphNode") / 37.0f;
}

float QtGraphViewStyleImpl::getCharHeightForNodeType(NodeType type)
{
	return QFontMetrics(QtGraphNode::getFontForNodeType(type)).height();
}

float QtGraphViewStyleImpl::getGraphViewZoomDifferenceForPlatform()
{
	if (utility::getOsType() == OS_MAC)
	{
		return 1;
	}

	return 1.25;
}
