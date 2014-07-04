#ifndef QT_GRAPH_VIEW_H
#define QT_GRAPH_VIEW_H

#include "component/view/GraphView.h"

class QGraphicsView;

class QtGraphView : public GraphView
{
public:
	QtGraphView(ViewLayout* viewLayout);
	virtual ~QtGraphView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initGui();

	virtual std::weak_ptr<GraphNode> addNode(const Vec2i& position, const std::string& name);
	virtual void addEdge(const std::weak_ptr<GraphNode>& owner, const std::weak_ptr<GraphNode>& target);

private:
	QGraphicsView* getView();
};

#endif // QT_GRAPH_VIEW_H
