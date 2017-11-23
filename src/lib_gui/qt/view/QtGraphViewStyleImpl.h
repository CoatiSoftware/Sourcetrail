#ifndef QT_GRAPH_VIEW_STYLE_IMPL_H
#define QT_GRAPH_VIEW_STYLE_IMPL_H

#include "component/view/GraphViewStyleImpl.h"

class QtGraphViewStyleImpl
	: public GraphViewStyleImpl
{
public:
	virtual ~QtGraphViewStyleImpl();
	virtual float getCharWidthForNodeType(NodeType type);
	virtual float getCharHeightForNodeType(NodeType type);
	virtual float getGraphViewZoomDifferenceForPlatform();
};

#endif // QT_GRAPH_VIEW_STYLE_IMPL_H
