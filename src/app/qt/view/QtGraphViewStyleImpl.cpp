#include "qt/view/QtGraphViewStyleImpl.h"

#include <QFontMetrics>

float QtGraphViewStyleImpl::getCharWidthForNodeType(Node::NodeType type)
{
	return QFontMetrics(QtGraphNode::getFontForNodeType(type)).width("QtGraphNode::QtGraphNode::QtGraphNode") / 37.0f;
}
