#include "qt/view/QtGraphViewStyleImpl.h"

#include <QFontMetrics>

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
