#ifndef QT_GRAPH_VIEW_H
#define QT_GRAPH_VIEW_H

#include "component/view/GraphView.h"

class QtGraphView : public GraphView
{
public:
	QtGraphView(ViewLayout* viewLayout);
	virtual ~QtGraphView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initGui();

	virtual void addNode(const Vec2i& position, const std::string& name);
};

#endif // QT_GRAPH_VIEW_H
