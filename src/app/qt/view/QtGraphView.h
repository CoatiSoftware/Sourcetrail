#ifndef QT_GRAPH_VIEW_H
#define QT_GRAPH_VIEW_H

#include "qt/utility/QtThreadedFunctor.h"
#include "utility/math/MatrixDynamicBase.h"
#include "utility/math/Vector4.h"
#include "utility/types.h"

#include "component/view/GraphView.h"

struct DummyEdge;
struct DummyNode;
class QGraphicsView;
class QtGraphEdge;
class QtGraphNode;

class QtGraphView: public GraphView
{
public:
	QtGraphView(ViewLayout* viewLayout);
	virtual ~QtGraphView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initView();
	virtual void refreshView();

	virtual void rebuildGraph(std::shared_ptr<Graph> graph, const std::vector<DummyNode>& nodes, const std::vector<DummyEdge>& edges);
	virtual void clear();

	virtual GraphView::Metrics getViewMetrics() const;

private:
	QGraphicsView* getView() const;

	void doRebuildGraph(std::shared_ptr<Graph> graph, const std::vector<DummyNode>& nodes, const std::vector<DummyEdge>& edges);
	void doClear();

	std::shared_ptr<QtGraphNode> findNodeRecursive(const std::list<std::shared_ptr<QtGraphNode>>& nodes, Id tokenId);

	std::shared_ptr<QtGraphNode> createNodeRecursive(
		QGraphicsView* view, std::shared_ptr<QtGraphNode> parentNode, const DummyNode& node);
	std::shared_ptr<QtGraphEdge> createEdge(QGraphicsView* view, const DummyEdge& edge);

	QtThreadedFunctor<std::shared_ptr<Graph>, const std::vector<DummyNode>&, const std::vector<DummyEdge>&> m_rebuildGraphFunctor;
	QtThreadedFunctor<void> m_clearFunctor;

	std::shared_ptr<Graph> m_graph;

	std::list<std::shared_ptr<QtGraphEdge>> m_edges;
	std::list<std::shared_ptr<QtGraphNode>> m_nodes;
};

#endif // QT_GRAPH_VIEW_H
