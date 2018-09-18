#ifndef QT_GRAPH_VIEW_H
#define QT_GRAPH_VIEW_H

#include <set>

#include <QGraphicsView>
#include <QPointF>

#include "GraphView.h"
#include "Graph.h"
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
{
	Q_OBJECT

public:
	QtGraphView(ViewLayout* viewLayout);
	virtual ~QtGraphView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initView();
	virtual void refreshView();

	// ScreenSearchResponder implementation
	virtual bool isVisible() const;
	virtual void findMatches(ScreenSearchSender* sender, const std::wstring& query);
	virtual void activateMatch(size_t matchIndex);
	virtual void deactivateMatch(size_t matchIndex);
	virtual void clearMatches();

	// GraphView implementation
	virtual void rebuildGraph(
		std::shared_ptr<Graph> graph,
		const std::vector<std::shared_ptr<DummyNode>>& nodes,
		const std::vector<std::shared_ptr<DummyEdge>>& edges,
		const GraphParams params);
	virtual void clear();

	virtual void focusTokenIds(const std::vector<Id>& focusedTokenIds);
	virtual void defocusTokenIds(const std::vector<Id>& defocusedTokenIds);

	virtual void resizeView();

	virtual Vec2i getViewSize() const;
	virtual GroupType getGrouping() const;

	virtual void scrollToValues(int xValue, int yValue);

	virtual void activateEdge(Id edgeId, bool centerOrigin);

private slots:
	void updateScrollBars();
	void finishedTransition();
	void clickedInEmptySpace();
	void pressedCharacterKey(QChar c);

	void scrolled(int);
	void resized();

	void trailDepthChanged(int);
	void trailDepthUpdated();

	void clickedCollapse();
	void clickedExpand();

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

	QtGraphNode* findNodeRecursive(const std::list<QtGraphNode*>& nodes, Id tokenId);

	QtGraphNode* createNodeRecursive(
		QGraphicsView* view, QtGraphNode* parentNode, const DummyNode* node, bool multipleActive, bool interactive);
	QtGraphEdge* createEdge(
		QGraphicsView* view, const DummyEdge* edge, std::set<Id>* visibleEdgeIds, Graph::TrailMode trailMode,
		QPointF pathOffset, bool useBezier, bool interactive);
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

#endif // QT_GRAPH_VIEW_H
