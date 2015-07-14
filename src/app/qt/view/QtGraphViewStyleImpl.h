#ifndef QT_GRAPH_VIEW_STYLE_IMPL_H
#define QT_GRAPH_VIEW_STYLE_IMPL_H

#include "component/view/GraphViewStyleImpl.h"
#include "qt/view/graphElements/QtGraphNode.h"

class QtGraphViewStyleImpl
	: public GraphViewStyleImpl
{
public:
	virtual float getCharWidthForNodeType(Node::NodeType type);
	virtual float getCharHeightForNodeType(Node::NodeType type);
};

#endif // QT_GRAPH_VIEW_STYLE_IMPL_H
