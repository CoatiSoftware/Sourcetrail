#ifndef GRAPH_VIEW_STYLE_IMPL_H
#define GRAPH_VIEW_STYLE_IMPL_H

#include "data/graph/Node.h"

class GraphViewStyleImpl
{
public:
	virtual ~GraphViewStyleImpl() { }
	virtual float getCharWidthForNodeType(Node::NodeType type) = 0;
	virtual float getCharHeightForNodeType(Node::NodeType type) = 0;
	virtual float getGraphViewZoomDifferenceForPlatform() = 0;
};

#endif // GRAPH_VIEW_STYLE_IMPL_H
