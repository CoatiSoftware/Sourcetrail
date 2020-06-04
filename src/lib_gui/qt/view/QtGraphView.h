#ifndef QT_GRAPH_VIEW_H
#define QT_GRAPH_VIEW_H

#include <set>

#include <QGraphicsView>
#include <QPointF>

#include "Graph.h"
#include "GraphFocusHandler.h"
#include "GraphView.h"
#include "QtScrollSpeedChangeListener.h"
#include "QtThreadedFunctor.h"
#include "types.h"

struct DummyEdge;
struct DummyNode;
class MessageActivateTrail;
class QLabel;
class QMouseEvent;
class QPushButton;
class QSequentialAnimationGroup;
class QSlider;
class QtGraphEdge;
class QtGraphicsView;
class QtGraphNode;
class QtSelfRefreshIconButton;

class QtGraphView
	: public QObject
	, public GraphView
	, public GraphFocusClient
{
	Q_OBJECT

public:
	QtGraphView(ViewLayout* viewLayout);
	~QtGraphView() = default;

	// View implementation
	void createWidgetWrapper() override;
	void refreshView() override;

	// ScreenSearchResponder implementation
	bool isVisible() const override;
	void findMatches(ScreenSearchSender* sender, const std::wstring& query) override;
	void activateMatch(size_t matchIndex) override;
	void deactivateMatch(size_t matchIndex) override;
	void clearMatches() override;

	// GraphView implementation
	void rebuildGraph(
		std::shared_ptr<Graph> graph,
		const std::vector<std::shared_ptr<DummyNode>>& nodes,
		const std::vector<std::shared_ptr<DummyEdge>>& edges,
		const GraphParams params) override;
	void clear() override;

	void coFocusTokenIds(const std::vector<Id>& focusedTokenIds) override;
	void deCoFocusTokenIds(const std::vector<Id>& defocusedTokenIds) override;

	void resizeView() override;

	Vec2i getViewSize() const override;
	GroupType getGrouping() const override;

	void scrollToValues(int xValue, int yValue) override;

	void activateEdge(Id edgeId) override;

	void setNavigationFocus(bool focusIn) override;
	bool hasNavigationFocus() const override;

	// GraphFocusClient implementation
	void focusView(bool focusIn) override;

	const std::list<QtGraphNode*>& getGraphNodes() const override;
	const std::list<QtGraphEdge*>& getGraphEdges() const override;

	QtGraphNode* getActiveNode() const override;

	void ensureNodeVisible(QtGraphNode* node) override;

private slots:
	void updateScrollBars();
	void finishedTransition();
	void clickedInEmptySpace();

	void scrolled(int);
	void resized();

	void trailDepthChanged(int);
	void trailDepthUpdated();

	void clickedCollapse();
	void clickedExpand();

	void clickedCustomTrail();
	void clickedBackwardTrail();
	void clickedForwardTrail();

	void groupingUpdated(QPushButton* button);

private:
	void performScroll(QScrollBar* scrollBar, int value) const;

	MessageActivateTrail getMessageActivateTrail(bool forward);
	void activateTrail(bool forward);
	void updateTrailButtons();

	void switchToNewGraphData();

	QtGraphicsView* getView() const;

	void doResize();

	QtGraphNode* createNodeRecursive(
		QGraphicsView* view,
		QtGraphNode* parentNode,
		const DummyNode* node,
		bool multipleActive,
		bool interactive);
	QtGraphEdge* createEdge(
		QGraphicsView* view,
		const DummyEdge* edge,
		std::set<Id>* visibleEdgeIds,
		Graph::TrailMode trailMode,
		QPointF pathOffset,
		bool useBezier,
		bool interactive);
	QtGraphEdge* createAggregationEdge(
		QGraphicsView* view, const DummyEdge* edge, std::set<Id>* visibleEdgeIds, bool interactive);

	QRectF itemsBoundingRect(const std::list<QtGraphNode*>& items) const;
	QRectF getSceneRect(const std::list<QtGraphNode*>& items) const;

	void centerNode(QtGraphNode* node);

	void compareNodesRecursive(
		std::list<QtGraphNode*> newSubNodes,
		std::list<QtGraphNode*> oldSubNodes,
		std::list<QtGraphNode*>* appearingNodes,
		std::list<QtGraphNode*>* vanishingNodes,
		std::vector<std::pair<QtGraphNode*, QtGraphNode*>>* remainingNodes);

	void createTransition();
	bool isTransitioning() const;

	GraphFocusHandler m_focusHandler;
	bool m_hasFocus = false;

	QtThreadedLambdaFunctor m_onQtThread;

	std::shared_ptr<Graph> m_graph;
	std::shared_ptr<Graph> m_oldGraph;

	std::list<QtGraphEdge*> m_edges;
	std::list<QtGraphEdge*> m_oldEdges;

	std::list<QtGraphNode*> m_nodes;
	std::list<QtGraphNode*> m_oldNodes;

	std::vector<QtGraphNode*> m_activeNodes;
	QtGraphNode* m_oldActiveNode = nullptr;

	bool m_centerActiveNode;
	bool m_scrollToTop;
	bool m_restoreScroll;
	Vec2i m_scrollValues;
	bool m_isIndexedList;

	std::shared_ptr<QSequentialAnimationGroup> m_transition;
	QPointF m_sceneRectOffset;

	QtScrollSpeedChangeListener m_scrollSpeedChangeListenerHorizontal;
	QtScrollSpeedChangeListener m_scrollSpeedChangeListenerVertical;

	QWidget* m_trailWidget;
	QtSelfRefreshIconButton* m_expandButton;
	QtSelfRefreshIconButton* m_collapseButton;
	QtSelfRefreshIconButton* m_customTrailButton;
	QtSelfRefreshIconButton* m_forwardTrailButton;
	QtSelfRefreshIconButton* m_backwardTrailButton;
	QSlider* m_trailDepthSlider;
	QLabel* m_trailDepthLabel;

	QWidget* m_groupWidget;
	QtSelfRefreshIconButton* m_groupFileButton;
	QtSelfRefreshIconButton* m_groupNamespaceButton;

	std::vector<QRectF> m_virtualNodeRects;

	// Name matches
	std::vector<QtGraphNode*> m_matchedNodes;
};

#endif	  // QT_GRAPH_VIEW_H
