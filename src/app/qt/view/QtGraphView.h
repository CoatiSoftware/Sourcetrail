#ifndef QT_GRAPH_VIEW_H
#define QT_GRAPH_VIEW_H

#include <QPointF>

#include "utility/types.h"

#include "qt/utility/QtThreadedFunctor.h"

#include "component/view/GraphView.h"

struct DummyEdge;
struct DummyNode;
class QGraphicsView;
class QSequentialAnimationGroup;
class QtGraphEdge;
class QtGraphNode;

class QtGraphView
	: public QObject
	, public GraphView
{
	Q_OBJECT

public:
	QtGraphView(ViewLayout* viewLayout);
	virtual ~QtGraphView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initView();
	virtual void refreshView();

	virtual void rebuildGraph(std::shared_ptr<Graph> graph, const std::vector<DummyNode>& nodes, const std::vector<DummyEdge>& edges);
	virtual void clear();

	virtual void focusToken(Id tokenId);
	virtual void defocusToken(Id tokenId);

	virtual void resizeView();

	virtual Vec2i getViewSize() const;

private slots:
	void centerScrollBars();
	void finishedTransition();

private:
	void switchToNewGraphData();

	QGraphicsView* getView() const;

	void doRebuildGraph(std::shared_ptr<Graph> graph, const std::vector<DummyNode>& nodes, const std::vector<DummyEdge>& edges);
	void doClear();
	void doResize();
	void doRefreshView();
	void doFocusIn(Id tokenId);
	void doFocusOut(Id tokeId);

	std::shared_ptr<QtGraphNode> findNodeRecursive(const std::list<std::shared_ptr<QtGraphNode>>& nodes, Id tokenId);

	std::shared_ptr<QtGraphNode> createNodeRecursive(
		QGraphicsView* view, std::shared_ptr<QtGraphNode> parentNode, const DummyNode& node);
	std::shared_ptr<QtGraphEdge> createEdge(QGraphicsView* view, const DummyEdge& edge);

	QRectF itemsBoundingRect(const std::list<std::shared_ptr<QtGraphNode>>& items) const;
	QRectF getSceneRect(const std::list<std::shared_ptr<QtGraphNode>>& items) const;

	void compareNodesRecursive(
		std::list<std::shared_ptr<QtGraphNode>> newSubNodes,
		std::list<std::shared_ptr<QtGraphNode>> oldSubNodes,
		std::list<QtGraphNode*>* appearingNodes,
		std::list<QtGraphNode*>* vanishingNodes,
		std::vector<std::pair<QtGraphNode*, QtGraphNode*>>* remainingNodes);

	void createTransition();

	QtThreadedFunctor<std::shared_ptr<Graph>, const std::vector<DummyNode>&, const std::vector<DummyEdge>&> m_rebuildGraphFunctor;
	QtThreadedFunctor<void> m_clearFunctor;
	QtThreadedFunctor<void> m_resizeFunctor;
	QtThreadedFunctor<void> m_refreshFunctor;
	QtThreadedFunctor<Id> m_focusInFunctor;
	QtThreadedFunctor<Id> m_focusOutFunctor;

	std::shared_ptr<Graph> m_graph;
	std::shared_ptr<Graph> m_oldGraph;

	std::list<std::shared_ptr<QtGraphEdge>> m_edges;
	std::list<std::shared_ptr<QtGraphEdge>> m_oldEdges;

	std::list<std::shared_ptr<QtGraphNode>> m_nodes;
	std::list<std::shared_ptr<QtGraphNode>> m_oldNodes;

	std::shared_ptr<QSequentialAnimationGroup> m_transition;
	QPointF m_sceneRectOffset;
};

#endif // QT_GRAPH_VIEW_H
