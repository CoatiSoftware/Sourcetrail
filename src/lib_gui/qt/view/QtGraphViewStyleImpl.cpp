#include "qt/view/QtGraphViewStyleImpl.h"

#include <QFontMetrics>
#include <QSysInfo>

#include "qt/view/graphElements/QtGraphNode.h"

QtGraphViewStyleImpl::~QtGraphViewStyleImpl()
{
}

float QtGraphViewStyleImpl::getCharWidthForNodeType(Node::NodeType type)
{
	return QFontMetrics(QtGraphNode::getFontForNodeType(type)).width("QtGraphNode::QtGraphNode::QtGraphNode") / 37.0f;
}

float QtGraphViewStyleImpl::getCharHeightForNodeType(Node::NodeType type)
{
	return QFontMetrics(QtGraphNode::getFontForNodeType(type)).height();
}

float QtGraphViewStyleImpl::getGraphViewZoomDifferenceForPlatform()
{
	if (QSysInfo::macVersion() == QSysInfo::MV_None)
	{
		return 1.25;
	}

	return 1;
}
