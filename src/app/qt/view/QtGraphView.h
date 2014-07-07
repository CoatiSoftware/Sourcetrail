#ifndef QT_GRAPH_VIEW_H
#define QT_GRAPH_VIEW_H

#include "qt/utility/QtThreadedFunctor.h"

#include "component/view/GraphView.h"

struct DummyEdge;
struct DummyNode;
class QGraphicsView;

class QtGraphView : public GraphView
{
public:
	QtGraphView(ViewLayout* viewLayout);
	virtual ~QtGraphView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initGui();

	virtual void rebuildGraph(const std::vector<DummyNode>& nodes, const std::vector<DummyEdge>& edges);

	virtual void clear();

private:
	QGraphicsView* getView();

	void doRebuildGraph(const std::vector<DummyNode>& nodes, const std::vector<DummyEdge>& edges);
	void doClear();

	std::shared_ptr<GraphNode> findOrCreateNode(QGraphicsView* view, const DummyNode& node);
	std::shared_ptr<GraphNode> findNode(const DummyNode& node);
	std::shared_ptr<GraphNode> createNode(QGraphicsView* view, const DummyNode& node);

	QtThreadedFunctor<const std::vector<DummyNode>&, const std::vector<DummyEdge>&> m_rebuildGraph;
	QtThreadedFunctor<void> m_clear;
};

#endif // QT_GRAPH_VIEW_H
