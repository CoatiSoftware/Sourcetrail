#ifndef QT_GRAPH_VIEW_STYLE_IMPL_H
#define QT_GRAPH_VIEW_STYLE_IMPL_H

#include "component/view/GraphViewStyleImpl.h"

class QtGraphViewStyleImpl
	: public GraphViewStyleImpl
{
public:
	virtual ~QtGraphViewStyleImpl();
	virtual float getCharWidth(NodeType::StyleType type) override;
	virtual float getCharHeight(NodeType::StyleType type) override;
	virtual float getGraphViewZoomDifferenceForPlatform() override;
};

#endif // QT_GRAPH_VIEW_STYLE_IMPL_H
