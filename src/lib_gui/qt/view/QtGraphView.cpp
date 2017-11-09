#include "qt/view/QtGraphView.h"

#include <QBoxLayout>
#include <QFrame>
#include <QGraphicsScene>
#include <QLabel>
#include <QMouseEvent>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScrollBar>
#include <QSequentialAnimationGroup>
#include <QStackedLayout>
#include <QSlider>

#include "component/controller/helper/DummyEdge.h"
#include "component/controller/helper/DummyNode.h"
#include "component/view/GraphViewStyle.h"
#include "settings/ApplicationSettings.h"
#include "utility/messaging/type/MessageActivateTrail.h"
#include "utility/messaging/type/MessageDeactivateEdge.h"
#include "utility/messaging/type/MessageScrollGraph.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/ResourcePaths.h"

#include "qt/graphics/QtGraphicsView.h"
#include "qt/utility/utilityQt.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "qt/view/graphElements/nodeComponents/QtGraphNodeComponentClickable.h"
#include "qt/view/graphElements/nodeComponents/QtGraphNodeComponentMoveable.h"
#include "qt/view/graphElements/QtGraphEdge.h"
#include "qt/view/graphElements/QtGraphNodeAccess.h"
#include "qt/view/graphElements/QtGraphNodeBundle.h"
#include "qt/view/graphElements/QtGraphNodeData.h"
#include "qt/view/graphElements/QtGraphNodeExpandToggle.h"
#include "qt/view/graphElements/QtGraphNodeQualifier.h"
#include "qt/view/graphElements/QtGraphNodeText.h"

QtGraphView::QtGraphView(ViewLayout* viewLayout)
	: GraphView(viewLayout)
	, m_centerActiveNode(false)
	, m_scrollToTop(false)
	, m_restoreScroll(false)
	, m_isIndexedList(false)
{
}

QtGraphView::~QtGraphView()
{
}

void QtGraphView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(new QFrame()));
}

void QtGraphView::initView()
{
	QWidget* widget = QtViewWidgetWrapper::getWidgetOfView(this);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	widget->setLayout(layout);

	QGraphicsScene* scene = new QGraphicsScene(widget);
	QtGraphicsView* view = new QtGraphicsView(widget);
	view->setScene(scene);
	view->setDragMode(QGraphicsView::ScrollHandDrag);
	view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	view->viewport()->setCursor(Qt::ArrowCursor);

	widget->layout()->addWidget(view);

	connect(view, &QtGraphicsView::emptySpaceClicked, this, &QtGraphView::clickedInEmptySpace);
	connect(view, &QtGraphicsView::characterKeyPressed, this, &QtGraphView::pressedCharacterKey);
	connect(view, &QtGraphicsView::resized, this, &QtGraphView::resized);

	m_scrollSpeedChangeListenerHorizontal.setScrollBar(view->horizontalScrollBar());
	m_scrollSpeedChangeListenerVertical.setScrollBar(view->verticalScrollBar());

	connect(view->horizontalScrollBar(), &QScrollBar::valueChanged, this, &QtGraphView::scrolled);
	connect(view->verticalScrollBar(), &QScrollBar::valueChanged, this, &QtGraphView::scrolled);

	// trail controls
	{
		QStackedLayout* stack = new QStackedLayout();

		{
			m_expandButton = new QPushButton();
			m_expandButton->setObjectName("expand_button");
			m_expandButton->setToolTip("show depth graph controls");
			m_expandButton->setGeometry(0, 0, 26, 26);
			connect(m_expandButton, &QPushButton::clicked, this, &QtGraphView::clickedExpand);
			stack->addWidget(m_expandButton);
		}

		{
			QWidget* ui = new QWidget();
			ui->setGeometry(0, 0, 26, 210);
			stack->addWidget(ui);

			m_collapseButton = new QPushButton(ui);
			m_collapseButton->setObjectName("collapse_button");
			m_collapseButton->setToolTip("hide depth graph controls");
			connect(m_collapseButton, &QPushButton::clicked, this, &QtGraphView::clickedCollapse);

			m_forwardTrailButton = new QPushButton(ui);
			m_forwardTrailButton->setObjectName("trail_button");
			connect(m_forwardTrailButton, &QPushButton::clicked, this, &QtGraphView::clickedForwardTrail);

			m_backwardTrailButton = new QPushButton(ui);
			m_backwardTrailButton->setObjectName("trail_button");
			connect(m_backwardTrailButton, &QPushButton::clicked, this, &QtGraphView::clickedBackwardTrail);

			m_trailDepthLabel = new QLabel(ui);
			m_trailDepthLabel->setObjectName("depth_label");
			m_trailDepthLabel->setToolTip("adjust graph depth");
			m_trailDepthLabel->setAlignment(Qt::AlignCenter);

			m_trailDepthSlider = new QSlider(Qt::Vertical, ui);
			m_trailDepthSlider->setObjectName("depth_slider");
			m_trailDepthSlider->setToolTip("adjust graph depth");
			m_trailDepthSlider->setMinimum(1);
			m_trailDepthSlider->setMaximum(26);
			m_trailDepthSlider->setValue(5);
			connect(m_trailDepthSlider, &QSlider::valueChanged, this, &QtGraphView::trailDepthChanged);

			m_collapseButton->setGeometry(0, 0, 26, 20);
			m_backwardTrailButton->setGeometry(0, 22, 26, 26);
			m_forwardTrailButton->setGeometry(0, 50, 26, 26);
			m_trailDepthLabel->setGeometry(0, 78, 26, 26);
			m_trailDepthSlider->setGeometry(0, 104, 26, 100);
		}

		m_trailWidget = new QWidget(widget);
		m_trailWidget->setGeometry(8, 8, 26, 210);
		m_trailWidget->setLayout(stack);

		if (ApplicationSettings::getInstance()->getGraphControlsVisible())
		{
			clickedExpand();
		}
		else
		{
			clickedCollapse();
		}

		updateTrailButtons();
		trailDepthChanged(0);
	}

	refreshView();
}

void QtGraphView::refreshView()
{
	m_onQtThread([this]()
	{
		doResize();

		QtGraphicsView* view = getView();

		std::string css = utility::getStyleSheet(ResourcePaths::getGuiPath().concat(FilePath("graph_view/graph_view.css")));
		view->setStyleSheet(css.c_str());
		view->setAppZoomFactor(GraphViewStyle::getZoomFactor());
		view->refreshStyle();

		m_trailWidget->setStyleSheet(css.c_str());

		m_expandButton->setIcon(utility::createButtonIcon(
			ResourcePaths::getGuiPath().str() + "graph_view/images/graph.png",
			"search/button"
		));

		m_collapseButton->setIcon(utility::createButtonIcon(
			ResourcePaths::getGuiPath().str() + "graph_view/images/graph_arrow.png",
			"search/button"
		));

		updateTrailButtons();
	});
}

bool QtGraphView::isVisible() const
{
	return QtViewWidgetWrapper::getWidgetOfView(this)->isVisible();
}

void QtGraphView::findMatches(ScreenSearchSender* sender, const std::string& query)
{
	m_onQtThread(
		[sender, query, this]()
		{
			m_matchedNodes.clear();

			for (std::shared_ptr<QtGraphNode> node : m_oldNodes)
			{
				node->matchNameRecursive(query, &m_matchedNodes);
			}

			sender->foundMatches(this, m_matchedNodes.size());
		}
	);
}

void QtGraphView::activateMatch(size_t matchIndex)
{
	m_onQtThread(
		[matchIndex, this]()
		{
			if (matchIndex >= m_matchedNodes.size())
			{
				return;
			}

			QtGraphNode* node = m_matchedNodes[matchIndex];

			node->setActiveMatch(true);
			node->updateStyle();

			centerNode(node);
		}
	);
}

void QtGraphView::deactivateMatch(size_t matchIndex)
{
	m_onQtThread(
		[matchIndex, this]()
		{
			if (matchIndex >= m_matchedNodes.size())
			{
				return;
			}

			QtGraphNode* node = m_matchedNodes[matchIndex];
			node->setActiveMatch(false);
			node->updateStyle();
		}
	);
}

void QtGraphView::clearMatches()
{
	m_onQtThread(
		[this]()
		{
			for (QtGraphNode* node : m_matchedNodes)
			{
				node->removeNameMatch();
			}

			m_matchedNodes.clear();
		}
	);
}

void QtGraphView::rebuildGraph(
	std::shared_ptr<Graph> graph,
	const std::vector<std::shared_ptr<DummyNode>>& nodes,
	const std::vector<std::shared_ptr<DummyEdge>>& edges,
	const GraphParams params
){
	m_onQtThread([=]()
	{
		if (m_transition && m_transition->currentTime() < m_transition->totalDuration())
		{
			m_transition->stop();
			m_transition.reset();
			finishedTransition();
		}

		if (graph)
		{
			m_graph = graph;
		}

		m_matchedNodes.clear();

		QGraphicsView* view = getView();


		// create nodes
		size_t activeNodeCount = 0;
		for (unsigned int i = 0; i < nodes.size(); i++)
		{
			activeNodeCount += nodes[i]->getActiveSubNodeCount();
		}

		m_nodes.clear();
		m_activeNodes.clear();
		m_oldActiveNode.reset();
		m_virtualNodeRects.clear();

		for (unsigned int i = 0; i < nodes.size(); i++)
		{
			std::shared_ptr<QtGraphNode> node = createNodeRecursive(view, NULL, nodes[i].get(), activeNodeCount > 1);
			if (node)
			{
				m_nodes.push_back(node);
			}
		}


		// move graph to center
		QPointF center = itemsBoundingRect(m_nodes).center();
		Vec2i o = GraphViewStyle::alignOnRaster(Vec2i(center.x(), center.y()));
		QPointF offset = QPointF(o.x, o.y);
		m_sceneRectOffset = offset - center;

		for (const std::shared_ptr<QtGraphNode>& node : m_nodes)
		{
			node->setPos(node->pos() - offset);
		}

		m_edges.clear();

		// create edges
		Graph::TrailMode trailMode = m_graph ? m_graph->getTrailMode() : Graph::TRAIL_NONE;
		std::set<Id> visibleEdgeIds;
		for (const std::shared_ptr<DummyEdge>& edge : edges)
		{
			if (!edge->data || !edge->data->isType(Edge::EDGE_AGGREGATION))
			{
				createEdge(view, edge.get(), &visibleEdgeIds, trailMode, offset, params.bezierEdges);
			}
		}
		for (const std::shared_ptr<DummyEdge>& edge : edges)
		{
			if (edge->data && edge->data->isType(Edge::EDGE_AGGREGATION))
			{
				createAggregationEdge(view, edge.get(), &visibleEdgeIds);
			}
		}

		m_centerActiveNode = params.centerActiveNode;
		m_scrollToTop = params.scrollToTop;
		m_isIndexedList = params.isIndexedList;

		if (params.animatedTransition && ApplicationSettings::getInstance()->getUseAnimations())
		{
			createTransition();
		}
		else
		{
			switchToNewGraphData();
		}
	});
}

void QtGraphView::clear()
{
	m_onQtThread([this]()
	{
		m_oldActiveNode.reset();
		m_activeNodes.clear();

		m_oldOldNodes = m_oldNodes;
		m_oldOldEdges = m_oldEdges;

		m_nodes.clear();
		m_edges.clear();

		m_oldNodes.clear();
		m_oldEdges.clear();

		m_graph.reset();
		m_oldGraph.reset();

		m_matchedNodes.clear();
	});

}

void QtGraphView::focusTokenIds(const std::vector<Id>& focusedTokenIds)
{
	m_onQtThread([=]()
	{
		for (const Id& tokenId : focusedTokenIds)
		{
			std::shared_ptr<QtGraphNode> node = findNodeRecursive(m_oldNodes, tokenId);
			if (node)
			{
				node->focusIn();
				continue;
			}

			for (std::shared_ptr<QtGraphEdge> edge : m_oldEdges)
			{
				if (edge->getData() && edge->getData()->getId() == tokenId)
				{
					edge->focusIn();
					break;
				}
			}
		}
	});
}

void QtGraphView::defocusTokenIds(const std::vector<Id>& defocusedTokenIds)
{
	m_onQtThread([=]()
	{
		for (const Id& tokenId : defocusedTokenIds)
		{
			std::shared_ptr<QtGraphNode> node = findNodeRecursive(m_oldNodes, tokenId);
			if (node && node->isDataNode())
			{
				node->focusOut();
				continue;
			}

			for (std::shared_ptr<QtGraphEdge> edge : m_oldEdges)
			{
				if (edge->getData() && edge->getData()->getId() == tokenId)
				{
					edge->focusOut();
					break;
				}
			}
		}
	});
}

void QtGraphView::resizeView()
{
	m_onQtThread([this]()
	{
		doResize();
	});
}

Vec2i QtGraphView::getViewSize() const
{
	QtGraphicsView* view = getView();

	float zoomFactor = view->getZoomFactor();
	return Vec2i((view->width() - 50) / zoomFactor, (view->height() - 100) / zoomFactor);
}

void QtGraphView::scrollToValues(int xValue, int yValue)
{
	m_restoreScroll = true;
	m_scrollValues = Vec2i(xValue, yValue);
}

void QtGraphView::activateEdge(Id edgeId, bool centerOrigin)
{
	m_onQtThread(
		[=]()
		{
			if (m_transition && m_transition->currentTime() < m_transition->totalDuration())
			{
				m_transition->stop();
				m_transition.reset();
				finishedTransition();
			}

			for (std::shared_ptr<QtGraphEdge>& edge : m_oldEdges)
			{
				edge->setIsActive(false);
				edge->setIsFocused(false);
			}

			for (std::shared_ptr<QtGraphEdge>& edge : m_oldEdges)
			{
				if (edge->getData() && edge->getData()->getId() == edgeId)
				{
					edge->setIsActive(true);

					if (centerOrigin)
					{
						centerNode(edge->getOwner().lock().get());
					}

					break;
				}
			}
		}
	);
}

void QtGraphView::updateScrollBars()
{
	QGraphicsView* view = getView();

	QScrollBar* hb = view->horizontalScrollBar();
	QScrollBar* vb = view->verticalScrollBar();

	if (m_restoreScroll)
	{
		hb->setValue(m_scrollValues.x());
		vb->setValue(m_scrollValues.y());
	}
	else if (m_scrollToTop)
	{
		vb->setValue(vb->minimum());
	}
	else
	{
		hb->setValue((hb->minimum() + hb->maximum()) / 2);
		vb->setValue((vb->minimum() + vb->maximum()) / 2);
	}
}

void QtGraphView::finishedTransition()
{
	QGraphicsView* view = getView();
	view->setInteractive(true);

	for (const std::shared_ptr<QtGraphNode>& node : m_nodes)
	{
		node->showNodeRecursive();
	}

	for (const std::shared_ptr<QtGraphEdge>& edge : m_edges)
	{
		edge->setOpacity(1.0f);
	}

	switchToNewGraphData();
}

void QtGraphView::clickedInEmptySpace()
{
	std::vector<std::shared_ptr<QtGraphEdge>> activeEdges;
	for (std::shared_ptr<QtGraphEdge>& edge : m_oldEdges)
	{
		if (edge->getIsActive())
		{
			activeEdges.push_back(edge);
		}

		edge->setIsFocused(false);
	}

	if (m_graph && m_graph->getTrailMode() != Graph::TRAIL_NONE)
	{
		for (std::shared_ptr<QtGraphEdge>& edge : activeEdges)
		{
			edge->setIsActive(false);
		}
	}
	else if (activeEdges.size() == 1)
	{
		MessageDeactivateEdge(false).dispatch();
	}
}

void QtGraphView::pressedCharacterKey(QChar c)
{
	if (!m_isIndexedList)
	{
		return;
	}

	QtGraphNode* node = nullptr;
	bool hasTextNodes = false;

	for (const std::shared_ptr<QtGraphNode>& n : m_oldNodes)
	{
		if (n->isTextNode() && n->getName().size())
		{
			hasTextNodes = true;
			QChar start(n->getName()[0]);
			if (start.toLower() >= c.toLower())
			{
				node = n.get();
				break;
			}
		}
	}

	if (!hasTextNodes)
	{
		return;
	}

	QtGraphicsView* view = getView();

	if (!node)
	{
		view->ensureVisibleAnimated(QRectF(0, view->scene()->height() - 5, view->scene()->width(), 5), 100, 100);
	}
	else
	{
		Vec2i pos = node->getPosition();
		Vec2i size = node->getSize();

		view->ensureVisibleAnimated(QRectF(pos.x, pos.y, size.x, size.y + getViewSize().y / 3 * 2), 100, 100);
	}
}

void QtGraphView::scrolled(int)
{
	QGraphicsView* view = getView();

	MessageScrollGraph(view->horizontalScrollBar()->value(), view->verticalScrollBar()->value()).dispatch();
}

void QtGraphView::resized()
{
	if (m_transition && m_transition->currentTime() < m_transition->totalDuration())
	{
		return;
	}

	doResize();
}

void QtGraphView::trailDepthChanged(int)
{
	if (m_trailDepthSlider->value() == m_trailDepthSlider->maximum())
	{
		m_trailDepthLabel->setText("inf");
	}
	else
	{
		m_trailDepthLabel->setText(QString::number(m_trailDepthSlider->value()));
	}
}

void QtGraphView::clickedCollapse()
{
	dynamic_cast<QStackedLayout*>(m_trailWidget->layout())->setCurrentIndex(0);

	ApplicationSettings::getInstance()->setGraphControlsVisible(false);
	ApplicationSettings::getInstance()->save();
}

void QtGraphView::clickedExpand()
{
	dynamic_cast<QStackedLayout*>(m_trailWidget->layout())->setCurrentIndex(1);

	ApplicationSettings::getInstance()->setGraphControlsVisible(true);
	ApplicationSettings::getInstance()->save();
}

void QtGraphView::clickedBackwardTrail()
{
	activateTrail(false);
}

void QtGraphView::clickedForwardTrail()
{
	activateTrail(true);
}

MessageActivateTrail QtGraphView::getMessageActivateTrail(bool forward)
{
	MessageActivateTrail message(0, 0, 0, 0, false);

	QtGraphNodeData* node = nullptr;
	if (m_oldActiveNode && (node = dynamic_cast<QtGraphNodeData*>(m_oldActiveNode.get())))
	{
		Edge::EdgeTypeMask trailType;
		bool horizontalLayout = true;

		switch (node->getData()->getType())
		{
			case Node::NODE_NON_INDEXED:
			case Node::NODE_CLASS:
			case Node::NODE_STRUCT:
			case Node::NODE_INTERFACE:
				trailType = Edge::EDGE_INHERITANCE;
				horizontalLayout = false;
				break;

			case Node::NODE_FUNCTION:
			case Node::NODE_METHOD:
				trailType = Edge::EDGE_CALL | Edge::EDGE_OVERRIDE |
					Edge::EDGE_TEMPLATE_SPECIALIZATION | Edge::EDGE_TEMPLATE_MEMBER_SPECIALIZATION;
				horizontalLayout = true;
				break;

			case Node::NODE_FILE:
				trailType = Edge::EDGE_INCLUDE;
				horizontalLayout = true;
				break;

			default:
				return message;
		}

		Id tokenId = node->getData()->getId();
		Id originId = forward ? tokenId : 0;
		Id targetId = forward ? 0 : tokenId;

		int depth = m_trailDepthSlider->value();
		if (depth == m_trailDepthSlider->maximum())
		{
			depth = 0;
		}

		return MessageActivateTrail(originId, targetId, trailType, depth, horizontalLayout);
	}

	return message;
}

void QtGraphView::activateTrail(bool forward)
{
	MessageActivateTrail message = getMessageActivateTrail(forward);
	if (message.trailType)
	{
		message.dispatch();
	}
}

void QtGraphView::updateTrailButtons()
{
	MessageActivateTrail message = getMessageActivateTrail(false);

	m_backwardTrailButton->setEnabled(message.trailType);
	m_forwardTrailButton->setEnabled(message.trailType);
	m_trailDepthLabel->setEnabled(message.trailType);
	m_trailDepthSlider->setEnabled(message.trailType);

	std::string backwardImagePath;
	std::string forwardImagePath;

	if (message.trailType & Edge::EDGE_CALL)
	{
		m_backwardTrailButton->setToolTip("show caller graph");
		m_forwardTrailButton->setToolTip("show callee graph");

		backwardImagePath = "graph_left.png";
		forwardImagePath = "graph_right.png";
	}
	else if (message.trailType & Edge::EDGE_INHERITANCE)
	{
		m_backwardTrailButton->setToolTip("show base hierarchy");
		m_forwardTrailButton->setToolTip("show derived hierarchy");

		backwardImagePath = "graph_up.png";
		forwardImagePath = "graph_down.png";
	}
	else if (message.trailType & Edge::EDGE_INCLUDE)
	{
		m_backwardTrailButton->setToolTip("show including files hierarchy");
		m_forwardTrailButton->setToolTip("show included files hierarchy");

		backwardImagePath = "graph_left.png";
		forwardImagePath = "graph_right.png";
	}
	else
	{
		m_backwardTrailButton->setToolTip("no depth graph available for active symbol");
		m_forwardTrailButton->setToolTip("no depth graph available for active symbol");

		backwardImagePath = "graph_left.png";
		forwardImagePath = "graph_right.png";
	}

	m_backwardTrailButton->setIcon(utility::createButtonIcon(
		ResourcePaths::getGuiPath().str() + "graph_view/images/" + backwardImagePath,
		"search/button"
	));

	m_forwardTrailButton->setIcon(utility::createButtonIcon(
		ResourcePaths::getGuiPath().str() + "graph_view/images/" + forwardImagePath,
		"search/button"
	));
}

void QtGraphView::switchToNewGraphData()
{
	m_oldGraph = m_graph;

	for (const std::shared_ptr<QtGraphNode>& node : m_oldNodes)
	{
		node->hide();
		node->setParentItem(nullptr);
	}

	for (const std::shared_ptr<QtGraphEdge>& edge : m_oldEdges)
	{
		edge->hide();
		edge->setParentItem(nullptr);
	}

	m_oldOldNodes = m_oldNodes;
	m_oldOldEdges = m_oldEdges;

	m_oldNodes = m_nodes;
	m_oldEdges = m_edges;

	m_nodes.clear();
	m_edges.clear();

	doResize();

	if (m_scrollToTop || m_restoreScroll)
	{
		updateScrollBars();

		m_scrollToTop = false;
		m_restoreScroll = false;
	}

	// Manually hover the item below the mouse cursor.
	QtGraphicsView* view = getView();
	QtGraphNode* node = view->getNodeAtCursorPosition();
	if (node)
	{
		node->hoverEnter();
	}

	if (m_activeNodes.size())
	{
		if (m_centerActiveNode)
		{
			centerNode(m_activeNodes.front().get());
		}

		if (m_activeNodes.size() == 1)
		{
			m_oldActiveNode = m_activeNodes.front();
		}
		m_activeNodes.clear();
	}

	// Repaint to make sure all artifacts are removed
	view->update();

	updateTrailButtons();

	if (m_oldGraph && m_oldGraph->getTrailMode() != Graph::TRAIL_NONE)
	{
		MessageStatus("Finished graph display").dispatch();
	}
}

QtGraphicsView* QtGraphView::getView() const
{
	QWidget* widget = QtViewWidgetWrapper::getWidgetOfView(this);

	QtGraphicsView* view = widget->findChild<QtGraphicsView*>("");

	if (!view)
	{
		LOG_ERROR("Failed to get QGraphicsView");
	}

	return view;
}

void QtGraphView::doResize()
{
	getView()->setSceneRect(getSceneRect(m_oldNodes));
}

std::shared_ptr<QtGraphNode> QtGraphView::findNodeRecursive(const std::list<std::shared_ptr<QtGraphNode>>& nodes, Id tokenId)
{
	for (const std::shared_ptr<QtGraphNode>& node : nodes)
	{
		if (node->getTokenId() == tokenId)
		{
			return node;
		}

		std::shared_ptr<QtGraphNode> result = findNodeRecursive(node->getSubNodes(), tokenId);
		if (result != NULL)
		{
			return result;
		}
	}

	return std::shared_ptr<QtGraphNode>();
}

std::shared_ptr<QtGraphNode> QtGraphView::createNodeRecursive(
	QGraphicsView* view, std::shared_ptr<QtGraphNode> parentNode, const DummyNode* node, bool multipleActive
){
	if (!node->visible)
	{
		return NULL;
	}

	std::shared_ptr<QtGraphNode> newNode;
	if (node->isGraphNode())
	{
		newNode = std::make_shared<QtGraphNodeData>(
			node->data, node->name, node->hasParent, node->childVisible, node->hasQualifier);
	}
	else if (node->isAccessNode())
	{
		newNode = std::make_shared<QtGraphNodeAccess>(node->accessKind);
	}
	else if (node->isExpandToggleNode())
	{
		newNode = std::make_shared<QtGraphNodeExpandToggle>(node->isExpanded(), node->invisibleSubNodeCount);
	}
	else if (node->isBundleNode())
	{
		newNode = std::make_shared<QtGraphNodeBundle>(
			node->tokenId, node->getBundledNodeCount(), node->bundledNodeType, node->name);
	}
	else if (node->isQualifierNode())
	{
		newNode = std::make_shared<QtGraphNodeQualifier>(node->qualifierName);
	}
	else if (node->isTextNode())
	{
		newNode = std::make_shared<QtGraphNodeText>(node->name);
	}

	newNode->setPosition(node->position);
	newNode->setSize(node->size);
	newNode->setIsActive(node->active);
	newNode->setMultipleActive(multipleActive);

	newNode->addComponent(std::make_shared<QtGraphNodeComponentClickable>(newNode));

	view->scene()->addItem(newNode.get());

	if (parentNode)
	{
		newNode->setParent(parentNode);
	}
	else if (!node->isTextNode())
	{
		newNode->addComponent(std::make_shared<QtGraphNodeComponentMoveable>(newNode));
	}

	if (node->active)
	{
		m_activeNodes.push_back(newNode);
	}

	for (unsigned int i = 0; i < node->subNodes.size(); i++)
	{
		std::shared_ptr<QtGraphNode> subNode = createNodeRecursive(view, newNode, node->subNodes[i].get(), multipleActive);
		if (subNode)
		{
			newNode->addSubNode(subNode);
		}
	}

	newNode->updateStyle();

	return newNode;
}

std::shared_ptr<QtGraphEdge> QtGraphView::createEdge(
	QGraphicsView* view,
	const DummyEdge* edge,
	std::set<Id>* visibleEdgeIds,
	Graph::TrailMode trailMode,
	QPointF pathOffset,
	bool useBezier)
{
	if (!edge->visible)
	{
		return NULL;
	}

	std::shared_ptr<QtGraphNode> owner = findNodeRecursive(m_nodes, edge->ownerId);
	std::shared_ptr<QtGraphNode> target = findNodeRecursive(m_nodes, edge->targetId);

	if (owner != NULL && target != NULL)
	{
		std::shared_ptr<QtGraphEdge> qtEdge =
			std::make_shared<QtGraphEdge>(owner, target, edge->data, edge->getWeight(), edge->active && !useBezier, edge->getDirection());

		if (trailMode != Graph::TRAIL_NONE)
		{
			std::vector<Vec4i> path = edge->path;
			for (size_t i = 0; i < path.size(); i++)
			{
				path[i].x = path[i].x - pathOffset.x();
				path[i].z = path[i].z - pathOffset.x();
				path[i].y = path[i].y - pathOffset.y();
				path[i].w = path[i].w - pathOffset.y();
			}

			for (const Vec4i& rect : path)
			{
				m_virtualNodeRects.push_back(QRectF(QPointF(rect.x(), rect.y()), QPointF(rect.z(), rect.w())));
			}

			qtEdge->setIsTrailEdge(path, trailMode == Graph::TRAIL_HORIZONTAL);
		}
		else if (useBezier)
		{
			qtEdge->setUseBezier(true);
		}

		qtEdge->updateLine();


		owner->addOutEdge(qtEdge);
		target->addInEdge(qtEdge);

		view->scene()->addItem(qtEdge.get());

		if (edge->data)
		{
			visibleEdgeIds->insert(edge->data->getId());
		}

		m_edges.push_back(qtEdge);

		return qtEdge;
	}

	return NULL;
}

std::shared_ptr<QtGraphEdge> QtGraphView::createAggregationEdge(
	QGraphicsView* view, const DummyEdge* edge, std::set<Id>* visibleEdgeIds)
{
	if (!edge->visible)
	{
		return NULL;
	}

	bool allVisible = true;
	std::set<Id> aggregationIds = edge->data->getComponent<TokenComponentAggregation>()->getAggregationIds();
	for (Id edgeId : aggregationIds)
	{
		if (visibleEdgeIds->find(edgeId) == visibleEdgeIds->end())
		{
			allVisible = false;
			break;
		}
	}

	if (allVisible)
	{
		return NULL;
	}

	return createEdge(view, edge, visibleEdgeIds, Graph::TRAIL_NONE, QPointF(), false);
}

QRectF QtGraphView::itemsBoundingRect(const std::list<std::shared_ptr<QtGraphNode>>& items) const
{
	QRectF boundingRect;
	for (const std::shared_ptr<QtGraphNode>& item : items)
	{
		boundingRect |= item->sceneBoundingRect();
	}
	return boundingRect;
}

QRectF QtGraphView::getSceneRect(const std::list<std::shared_ptr<QtGraphNode>>& items) const
{
	QRectF sceneRect = itemsBoundingRect(items);

	for (const QRectF& rect : m_virtualNodeRects)
	{
		sceneRect |= rect;
	}

	return sceneRect.adjusted(-75, -75, 75, 75).translated(m_sceneRectOffset);
}

void QtGraphView::centerNode(QtGraphNode* node)
{
	QtGraphicsView* view = getView();

	Vec2i pos = node->getPosition();
	Vec2i size = node->getSize();

	QRectF rect(pos.x, pos.y, size.x, size.y);

	if (rect.height() > view->height() - 200)
	{
		rect.setHeight(view->height() - 200);
	}

	view->ensureVisibleAnimated(rect, 100, 100);
}

void QtGraphView::compareNodesRecursive(
	std::list<std::shared_ptr<QtGraphNode>> newSubNodes,
	std::list<std::shared_ptr<QtGraphNode>> oldSubNodes,
	std::list<QtGraphNode*>* appearingNodes,
	std::list<QtGraphNode*>* vanishingNodes,
	std::vector<std::pair<QtGraphNode*, QtGraphNode*>>* remainingNodes
){
	for (std::list<std::shared_ptr<QtGraphNode>>::iterator it = newSubNodes.begin(); it != newSubNodes.end(); it++)
	{
		bool remains = false;

		for (std::list<std::shared_ptr<QtGraphNode>>::iterator it2 = oldSubNodes.begin(); it2 != oldSubNodes.end(); it2++)
		{
			if (((*it)->isDataNode() && (*it2)->isDataNode() && (*it)->getTokenId() == (*it2)->getTokenId()) ||
				((*it)->isAccessNode() && (*it2)->isAccessNode() &&
					dynamic_cast<QtGraphNodeAccess*>((*it).get())->getAccessKind() ==
						dynamic_cast<QtGraphNodeAccess*>((*it2).get())->getAccessKind()) ||
				((*it)->isExpandToggleNode() && (*it2)->isExpandToggleNode()) ||
				((*it)->isBundleNode() && (*it2)->isBundleNode() && (*it)->getTokenId() == (*it2)->getTokenId()) ||
				((*it)->isQualifierNode() && (*it2)->isQualifierNode() && (*it)->getTokenId() == (*it2)->getTokenId()))
			{
				remainingNodes->push_back(std::pair<QtGraphNode*, QtGraphNode*>((*it).get(), (*it2).get()));
				compareNodesRecursive(
					(*it)->getSubNodes(), (*it2)->getSubNodes(), appearingNodes, vanishingNodes, remainingNodes);

				oldSubNodes.erase(it2);
				remains = true;
				break;
			}
		}

		if (!remains)
		{
			appearingNodes->push_back((*it).get());
		}
	}

	for (std::shared_ptr<QtGraphNode>& node : oldSubNodes)
	{
		vanishingNodes->push_back(node.get());
	}
}

void QtGraphView::createTransition()
{
	std::list<QtGraphNode*> appearingNodes;
	std::list<QtGraphNode*> vanishingNodes;
	std::vector<std::pair<QtGraphNode*, QtGraphNode*>> remainingNodes;

	compareNodesRecursive(m_nodes, m_oldNodes, &appearingNodes, &vanishingNodes, &remainingNodes);

	if (!vanishingNodes.size() && !appearingNodes.size())
	{
		bool nodesMoved = false;
		for (const std::pair<QtGraphNode*, QtGraphNode*>& p : remainingNodes)
		{
			if (p.first->getPosition() != p.second->getPosition() && p.first->getSize() != p.second->getSize())
			{
				nodesMoved = true;
			}
		}

		if (!nodesMoved)
		{
			switchToNewGraphData();
			return;
		}
	}

	QGraphicsView* view = getView();
	view->setInteractive(false);

	m_transition = std::make_shared<QSequentialAnimationGroup>();

	// fade out
	if (vanishingNodes.size() || m_oldEdges.size())
	{
		QParallelAnimationGroup* vanish = new QParallelAnimationGroup();

		for (QtGraphNode* node : vanishingNodes)
		{
			QPropertyAnimation* anim = new QPropertyAnimation(node, "opacity");
			anim->setDuration(300);
			anim->setStartValue(1.0f);
			anim->setEndValue(0.0f);

			vanish->addAnimation(anim);
		}

		for (const std::shared_ptr<QtGraphEdge>& edge : m_oldEdges)
		{
			QPropertyAnimation* anim = new QPropertyAnimation(edge.get(), "opacity");
			anim->setDuration(150);
			anim->setStartValue(1.0f);
			anim->setEndValue(0.0f);

			vanish->addAnimation(anim);
		}

		m_transition->addAnimation(vanish);
	}

	// move and scale
	{
		QParallelAnimationGroup* remain = new QParallelAnimationGroup();

		for (std::pair<QtGraphNode*, QtGraphNode*> p : remainingNodes)
		{
			QtGraphNode* newNode = p.first;
			QtGraphNode* oldNode = p.second;

			QPropertyAnimation* anim = new QPropertyAnimation(oldNode, "pos");
			anim->setDuration(300);
			anim->setStartValue(oldNode->pos());
			anim->setEndValue(newNode->pos());

			remain->addAnimation(anim);

			connect(anim, &QPropertyAnimation::finished, newNode, &QtGraphNode::showNode);
			connect(anim, &QPropertyAnimation::finished, oldNode, &QtGraphNode::hideNode);
			newNode->hide();

			anim = new QPropertyAnimation(oldNode, "size");
			anim->setDuration(300);
			anim->setStartValue(oldNode->size());
			anim->setEndValue(newNode->size());

			remain->addAnimation(anim);

			if (newNode->isAccessNode() && newNode->getSubNodes().size() == 0 && oldNode->getSubNodes().size() > 0)
			{
				dynamic_cast<QtGraphNodeAccess*>(oldNode)->hideLabel();
			}
		}

		QPropertyAnimation* anim = new QPropertyAnimation(view, "sceneRect");
		anim->setStartValue(view->sceneRect());
		anim->setEndValue(getSceneRect(m_nodes));

		anim->setDuration(300);

		if (!remainingNodes.size() || m_scrollToTop || m_restoreScroll)
		{
			connect(anim, &QPropertyAnimation::finished, this, &QtGraphView::updateScrollBars);
		}

		remain->addAnimation(anim);

		m_transition->addAnimation(remain);
	}

	// fade in
	if (appearingNodes.size() || m_edges.size())
	{
		QParallelAnimationGroup* appear = new QParallelAnimationGroup();

		for (QtGraphNode* node : appearingNodes)
		{
			QPropertyAnimation* anim = new QPropertyAnimation(node, "opacity");
			anim->setDuration(300);
			anim->setStartValue(0.0f);
			anim->setEndValue(1.0f);

			appear->addAnimation(anim);

			connect(anim, &QPropertyAnimation::finished, node, &QtGraphNode::blendIn);
			node->blendOut();
		}

		for (const std::shared_ptr<QtGraphEdge>& edge : m_edges)
		{
			QPropertyAnimation* anim = new QPropertyAnimation(edge.get(), "opacity");
			anim->setDuration(150);
			anim->setStartValue(0.0f);
			anim->setEndValue(1.0f);

			appear->addAnimation(anim);

			edge->setOpacity(0.0f);
		}

		m_transition->addAnimation(appear);
	}

	connect(m_transition.get(), &QPropertyAnimation::finished, this, &QtGraphView::finishedTransition);
	m_transition->start();
}
