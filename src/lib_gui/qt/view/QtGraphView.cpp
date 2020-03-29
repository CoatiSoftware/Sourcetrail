#include "QtGraphView.h"

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
#include <QSlider>
#include <QStackedLayout>

#include "ApplicationSettings.h"
#include "DummyEdge.h"
#include "DummyNode.h"
#include "GraphViewStyle.h"
#include "MessageActivateTrail.h"
#include "MessageCustomTrailShow.h"
#include "MessageDeactivateEdge.h"
#include "MessageRefreshUI.h"
#include "MessageScrollGraph.h"
#include "MessageStatus.h"
#include "QtGraphEdge.h"
#include "QtGraphNodeAccess.h"
#include "QtGraphNodeBundle.h"
#include "QtGraphNodeComponentClickable.h"
#include "QtGraphNodeComponentMoveable.h"
#include "QtGraphNodeData.h"
#include "QtGraphNodeExpandToggle.h"
#include "QtGraphNodeGroup.h"
#include "QtGraphNodeQualifier.h"
#include "QtGraphNodeText.h"
#include "QtGraphicsView.h"
#include "QtSelfRefreshIconButton.h"
#include "QtViewWidgetWrapper.h"
#include "ResourcePaths.h"
#include "utilityQt.h"

QtGraphView::QtGraphView(ViewLayout* viewLayout)
	: GraphView(viewLayout)
	, m_focusHandler(this)
	, m_centerActiveNode(false)
	, m_scrollToTop(false)
	, m_restoreScroll(false)
	, m_isIndexedList(false)
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(new QFrame()));

	QWidget* widget = QtViewWidgetWrapper::getWidgetOfView(this);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	widget->setLayout(layout);

	QGraphicsScene* scene = new QGraphicsScene(widget);
	QtGraphicsView* view = new QtGraphicsView(&m_focusHandler, widget);
	view->setScene(scene);
	view->setDragMode(QGraphicsView::ScrollHandDrag);
	view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	view->viewport()->setCursor(Qt::ArrowCursor);

	widget->layout()->addWidget(view);

	connect(view, &QtGraphicsView::emptySpaceClicked, this, &QtGraphView::clickedInEmptySpace);
	connect(view, &QtGraphicsView::resized, this, &QtGraphView::resized);
	connect(view, &QtGraphicsView::focusIn, [this](){ setNavigationFocus(true); });
	connect(view, &QtGraphicsView::focusOut, [this](){ setNavigationFocus(false); });

	m_scrollSpeedChangeListenerHorizontal.setScrollBar(view->horizontalScrollBar());
	m_scrollSpeedChangeListenerVertical.setScrollBar(view->verticalScrollBar());

	connect(view->horizontalScrollBar(), &QScrollBar::valueChanged, this, &QtGraphView::scrolled);
	connect(view->verticalScrollBar(), &QScrollBar::valueChanged, this, &QtGraphView::scrolled);

	// trail controls
	{
		QStackedLayout* stack = new QStackedLayout();

		{
			m_expandButton = new QtSelfRefreshIconButton(
				QLatin1String(""),
				ResourcePaths::getGuiPath().concatenate(L"graph_view/images/graph.png"),
				"search/button");
			m_expandButton->setObjectName(QStringLiteral("expand_button"));
			m_expandButton->setToolTip(QStringLiteral("show trail controls"));
			m_expandButton->setIconSize(QSize(16, 16));
			m_expandButton->setGeometry(0, 0, 26, 26);
			connect(m_expandButton, &QPushButton::clicked, this, &QtGraphView::clickedExpand);
			stack->addWidget(m_expandButton);
		}

		{
			QWidget* ui = new QWidget();
			ui->setGeometry(0, 0, 26, 210);
			stack->addWidget(ui);

			m_collapseButton = new QtSelfRefreshIconButton(
				QLatin1String(""),
				ResourcePaths::getGuiPath().concatenate(L"graph_view/images/graph_arrow.png"),
				"search/button",
				ui);
			m_collapseButton->setObjectName(QStringLiteral("collapse_button"));
			m_collapseButton->setToolTip(QStringLiteral("hide trail controls"));
			m_collapseButton->setIconSize(QSize(16, 16));
			connect(m_collapseButton, &QPushButton::clicked, this, &QtGraphView::clickedCollapse);

			m_customTrailButton = new QtSelfRefreshIconButton(
				QLatin1String(""), FilePath(), "search/button", ui);
			m_customTrailButton->setObjectName(QStringLiteral("trail_button"));
			m_customTrailButton->setIconSize(QSize(16, 16));
			m_customTrailButton->setToolTip(QStringLiteral("custom trail"));
			m_customTrailButton->setIconPath(
				ResourcePaths::getGuiPath().concatenate(L"graph_view/images/graph_custom.png"));
			connect(
				m_customTrailButton, &QPushButton::clicked, this, &QtGraphView::clickedCustomTrail);

			m_forwardTrailButton = new QtSelfRefreshIconButton(
				QLatin1String(""), FilePath(), "search/button", ui);
			m_forwardTrailButton->setObjectName(QStringLiteral("trail_button"));
			m_forwardTrailButton->setIconSize(QSize(16, 16));
			connect(
				m_forwardTrailButton, &QPushButton::clicked, this, &QtGraphView::clickedForwardTrail);

			m_backwardTrailButton = new QtSelfRefreshIconButton(
				QLatin1String(""), FilePath(), "search/button", ui);
			m_backwardTrailButton->setObjectName(QStringLiteral("trail_button"));
			m_backwardTrailButton->setIconSize(QSize(16, 16));
			connect(
				m_backwardTrailButton, &QPushButton::clicked, this, &QtGraphView::clickedBackwardTrail);

			m_trailDepthLabel = new QLabel(ui);
			m_trailDepthLabel->setObjectName(QStringLiteral("depth_label"));
			m_trailDepthLabel->setToolTip(QStringLiteral("adjust trail depth"));
			m_trailDepthLabel->setAlignment(Qt::AlignCenter);

			m_trailDepthSlider = new QSlider(Qt::Vertical, ui);
			m_trailDepthSlider->setObjectName(QStringLiteral("depth_slider"));
			m_trailDepthSlider->setToolTip(QStringLiteral("adjust trail depth"));
			m_trailDepthSlider->setMinimum(1);
			m_trailDepthSlider->setMaximum(26);
			m_trailDepthSlider->setValue(5);
			connect(
				m_trailDepthSlider, &QSlider::valueChanged, this, &QtGraphView::trailDepthChanged);
			connect(
				m_trailDepthSlider, &QSlider::sliderReleased, this, &QtGraphView::trailDepthUpdated);

			m_collapseButton->setGeometry(0, 0, 26, 20);
			m_customTrailButton->setGeometry(0, 22, 26, 26);
			m_backwardTrailButton->setGeometry(0, 50, 26, 26);
			m_forwardTrailButton->setGeometry(0, 78, 26, 26);
			m_trailDepthLabel->setGeometry(0, 106, 26, 26);
			m_trailDepthSlider->setGeometry(0, 132, 26, 100);
		}

		m_trailWidget = new QWidget(widget);
		m_trailWidget->setGeometry(8, 8, 26, 238);
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

	// group controls
	{
		m_groupFileButton = new QtSelfRefreshIconButton(
			QLatin1String(""),
			ResourcePaths::getGuiPath().concatenate(L"graph_view/images/file.png"),
			"search/button");
		m_groupNamespaceButton = new QtSelfRefreshIconButton(
			QLatin1String(""),
			ResourcePaths::getGuiPath().concatenate(L"graph_view/images/group_namespace.png"),
			"search/button");

		m_groupFileButton->setObjectName(QStringLiteral("group_right_button"));
		m_groupNamespaceButton->setObjectName(QStringLiteral("group_left_button"));

		m_groupFileButton->setToolTip(QStringLiteral("group by file"));
		m_groupNamespaceButton->setToolTip(QStringLiteral("group by package/namespace/module"));

		m_groupFileButton->setCheckable(true);
		m_groupNamespaceButton->setCheckable(true);

		m_groupFileButton->setIconSize(QSize(14, 14));
		m_groupNamespaceButton->setIconSize(QSize(14, 14));

		connect(m_groupFileButton, &QPushButton::clicked, [this]() {
			groupingUpdated(m_groupFileButton);
		});
		connect(m_groupNamespaceButton, &QPushButton::clicked, [this]() {
			groupingUpdated(m_groupNamespaceButton);
		});

		GroupType type = ApplicationSettings::getInstance()->getGraphGrouping();
		if (type == GroupType::FILE)
		{
			m_groupFileButton->setChecked(true);
		}
		else if (type == GroupType::NAMESPACE)
		{
			m_groupNamespaceButton->setChecked(true);
		}


		m_groupWidget = new QWidget(widget);
		m_groupWidget->setGeometry(38, 8, 54, 26);

		QHBoxLayout* layout = new QHBoxLayout();
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(2);

		layout->addWidget(m_groupNamespaceButton);
		layout->addWidget(m_groupFileButton);

		m_groupWidget->setLayout(layout);
	}
}

void QtGraphView::createWidgetWrapper() {}

void QtGraphView::refreshView()
{
	m_onQtThread([this]() {
		doResize();

		QtGraphicsView* view = getView();

		const std::string css = utility::getStyleSheet(
			ResourcePaths::getGuiPath().concatenate(L"graph_view/graph_view.css"));
		view->setStyleSheet(css.c_str());
		view->setAppZoomFactor(GraphViewStyle::getZoomFactor());

		m_trailWidget->setStyleSheet(css.c_str());
		m_groupWidget->setStyleSheet(css.c_str());

		updateTrailButtons();
	});
}

bool QtGraphView::isVisible() const
{
	return QtViewWidgetWrapper::getWidgetOfView(this)->isVisible();
}

void QtGraphView::findMatches(ScreenSearchSender* sender, const std::wstring& query)
{
	m_onQtThread([sender, query, this]() {
		m_matchedNodes.clear();

		for (QtGraphNode* node: m_oldNodes)
		{
			node->matchNameRecursive(query, &m_matchedNodes);
		}

		sender->foundMatches(this, m_matchedNodes.size());
	});
}

void QtGraphView::activateMatch(size_t matchIndex)
{
	m_onQtThread([matchIndex, this]() {
		if (matchIndex >= m_matchedNodes.size())
		{
			return;
		}

		QtGraphNode* node = m_matchedNodes[matchIndex];

		node->setActiveMatch(true);
		node->updateStyle();

		centerNode(node);
	});
}

void QtGraphView::deactivateMatch(size_t matchIndex)
{
	m_onQtThread([matchIndex, this]() {
		if (matchIndex >= m_matchedNodes.size())
		{
			return;
		}

		QtGraphNode* node = m_matchedNodes[matchIndex];
		node->setActiveMatch(false);
		node->updateStyle();
	});
}

void QtGraphView::clearMatches()
{
	if (m_matchedNodes.empty())
	{
		return;
	}

	m_onQtThread([this]() {
		for (QtGraphNode* node: m_matchedNodes)
		{
			node->removeNameMatch();
		}

		m_matchedNodes.clear();
	});
}

void QtGraphView::rebuildGraph(
	std::shared_ptr<Graph> graph,
	const std::vector<std::shared_ptr<DummyNode>>& nodes,
	const std::vector<std::shared_ptr<DummyEdge>>& edges,
	const GraphParams params)
{
	m_onQtThread([=]() {
		if (isTransitioning())
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

		Id oldActiveTokenId = m_oldActiveNode ? m_oldActiveNode->getTokenId() : 0;
		m_nodes.clear();
		m_activeNodes.clear();
		m_oldActiveNode = nullptr;
		m_virtualNodeRects.clear();

		for (unsigned int i = 0; i < nodes.size(); i++)
		{
			QtGraphNode* node = createNodeRecursive(
				view, nullptr, nodes[i].get(), activeNodeCount > 1, !params.disableInteraction);
			if (node)
			{
				m_nodes.push_back(node);
			}
		}

		if (m_activeNodes.size() == 1)
		{
			m_oldActiveNode = m_activeNodes.front();
		}

		Id newActiveTokenId = m_oldActiveNode ? m_oldActiveNode->getTokenId() : 0;

		// move graph to center
		QPointF center = itemsBoundingRect(m_nodes).center();
		const Vec2i o = GraphViewStyle::alignOnRaster(
			Vec2i(static_cast<int>(center.x()), static_cast<int>(center.y())));
		QPointF offset = QPointF(o.x, o.y);
		m_sceneRectOffset = offset - center;

		for (QtGraphNode* node: m_nodes)
		{
			node->setPos(node->pos() - offset);
		}

		m_edges.clear();
		QtGraphEdge::clearFocusedEdges();

		// create edges
		Graph::TrailMode trailMode = m_graph ? m_graph->getTrailMode() : Graph::TRAIL_NONE;
		std::set<Id> visibleEdgeIds;
		for (const std::shared_ptr<DummyEdge>& edge: edges)
		{
			if (!edge->data || !edge->data->isType(Edge::EDGE_AGGREGATION))
			{
				createEdge(
					view,
					edge.get(),
					&visibleEdgeIds,
					trailMode,
					offset,
					params.bezierEdges,
					!params.disableInteraction);
			}
		}
		for (const std::shared_ptr<DummyEdge>& edge: edges)
		{
			if (edge->data && edge->data->isType(Edge::EDGE_AGGREGATION))
			{
				createAggregationEdge(view, edge.get(), &visibleEdgeIds, !params.disableInteraction);
			}
		}

		// focus previously focused node
		if (params.tokenIdToFocus)
		{
			m_focusHandler.focusTokenId(m_nodes, m_edges, params.tokenIdToFocus);
		}
		else if (hasNavigationFocus())
		{
			m_focusHandler.refocusNode(m_nodes, oldActiveTokenId, newActiveTokenId);
		}
		else
		{
			m_focusHandler.clear();
		}

		m_centerActiveNode = params.centerActiveNode;
		m_scrollToTop = params.scrollToTop;
		m_isIndexedList = params.isIndexedList;

		if (params.animatedTransition && ApplicationSettings::getInstance()->getUseAnimations() &&
			view->isVisible())
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
	m_onQtThread([this]() {
		m_focusHandler.clear();

		m_oldActiveNode = nullptr;
		m_activeNodes.clear();

		for (QtGraphNode* node: m_oldNodes)
		{
			node->deleteLater();
		}

		for (QtGraphEdge* edge: m_oldEdges)
		{
			edge->deleteLater();
		}

		m_nodes.clear();
		m_edges.clear();

		m_oldNodes.clear();
		m_oldEdges.clear();

		m_graph.reset();
		m_oldGraph.reset();

		m_matchedNodes.clear();

		getView()->scene()->clear();
	});
}

void QtGraphView::coFocusTokenIds(const std::vector<Id>& focusedTokenIds)
{
	m_onQtThread([=]() {
		for (const Id& tokenId: focusedTokenIds)
		{
			QtGraphNode* node = QtGraphNode::findNodeRecursive(m_oldNodes, tokenId);
			if (node && !node->getIsFocused())
			{
				node->coFocusIn();
				continue;
			}

			for (QtGraphEdge* edge: m_oldEdges)
			{
				if (edge->getData() && edge->getData()->getId() == tokenId)
				{
					edge->coFocusIn();
					break;
				}
			}
		}
	});
}

void QtGraphView::deCoFocusTokenIds(const std::vector<Id>& defocusedTokenIds)
{
	m_onQtThread([=]() {
		for (const Id& tokenId: defocusedTokenIds)
		{
			QtGraphNode* node = QtGraphNode::findNodeRecursive(m_oldNodes, tokenId);
			if (node && !node->getIsFocused())
			{
				node->coFocusOut();
				continue;
			}

			for (QtGraphEdge* edge: m_oldEdges)
			{
				if (edge->getData() && edge->getData()->getId() == tokenId)
				{
					edge->coFocusOut();
					break;
				}
			}
		}
	});
}

void QtGraphView::resizeView()
{
	m_onQtThread([this]() { doResize(); });
}

Vec2i QtGraphView::getViewSize() const
{
	QtGraphicsView* view = getView();

	const float zoomFactor = view->getZoomFactor();
	return Vec2i(
		static_cast<int>((view->width() - 50) / zoomFactor),
		static_cast<int>((view->height() - 100) / zoomFactor));
}

GroupType QtGraphView::getGrouping() const
{
	if (m_groupFileButton->isChecked())
	{
		return GroupType::FILE;
	}
	else if (m_groupNamespaceButton->isChecked())
	{
		return GroupType::NAMESPACE;
	}

	return GroupType::NONE;
}

void QtGraphView::scrollToValues(int xValue, int yValue)
{
	m_restoreScroll = true;
	m_scrollValues = Vec2i(xValue, yValue);
}

void QtGraphView::activateEdge(Id edgeId)
{
	m_onQtThread([=]() {
		if (isTransitioning())
		{
			m_transition->stop();
			m_transition.reset();
			finishedTransition();
		}

		for (QtGraphEdge* edge: m_oldEdges)
		{
			edge->setIsActive(false);
		}

		for (QtGraphEdge* edge: m_oldEdges)
		{
			if (edge->getData() && edge->getData()->getId() == edgeId)
			{
				edge->setIsActive(true);
				break;
			}
		}
	});
}

void QtGraphView::setNavigationFocus(bool focus)
{
	if (m_hasFocus == focus)
	{
		return;
	}

	m_hasFocus = focus;

	m_onQtThread([this, focus]() {
		focusView(focus);

		if (focus)
		{
			m_focusHandler.focus();
		}
		else
		{
			m_focusHandler.defocus();
		}
	});
}

bool QtGraphView::hasNavigationFocus() const
{
	return m_hasFocus;
}

void QtGraphView::focusView(bool focusIn)
{
	QtGraphicsView* view = getView();

	view->blockSignals(true);

	if (focusIn)
	{
		view->setFocus();
	}
	else
	{
		view->clearFocus();
	}

	view->blockSignals(false);
}

const std::list<QtGraphNode*>& QtGraphView::getGraphNodes() const
{
	if (isTransitioning())
	{
		return m_nodes;
	}

	return m_oldNodes;
}

const std::list<QtGraphEdge*>& QtGraphView::getGraphEdges() const
{
	if (isTransitioning())
	{
		return m_edges;
	}

	return m_oldEdges;
}

QtGraphNode* QtGraphView::getActiveNode() const
{
	return m_oldActiveNode;
}

void QtGraphView::ensureNodeVisible(QtGraphNode* node)
{
	QtGraphicsView* view = getView();

	Vec4i r = node->getBoundingRect();
	QRectF rect(r.x(), r.y(), r.z() - r.x(), r.w() - r.y());

	if (rect.width() > view->width() - 100)
	{
		rect.setWidth(view->width() - 100);
	}

	if (rect.height() > view->height() - 100)
	{
		rect.setHeight(view->height() - 100);
	}

	view->ensureVisibleAnimated(rect, 100, 100);
}

void QtGraphView::updateScrollBars()
{
	QGraphicsView* view = getView();

	QScrollBar* hb = view->horizontalScrollBar();
	QScrollBar* vb = view->verticalScrollBar();

	if (m_restoreScroll)
	{
		performScroll(hb, m_scrollValues.x());
		performScroll(vb, m_scrollValues.y());
	}
	else if (m_scrollToTop)
	{
		vb->setValue(vb->minimum());
	}
	else if (!m_centerActiveNode)
	{
		performScroll(hb, (hb->minimum() + hb->maximum()) / 2);
		performScroll(vb, (vb->minimum() + vb->maximum()) / 2);
	}

	if (m_scrollToTop || m_restoreScroll)
	{
		m_centerActiveNode = false;
	}

	m_restoreScroll = false;
	m_scrollToTop = false;
}

void QtGraphView::finishedTransition()
{
	QGraphicsView* view = getView();
	view->setInteractive(true);

	for (QtGraphNode* node: m_nodes)
	{
		node->showNodeRecursive();
	}

	for (QtGraphEdge* edge: m_edges)
	{
		edge->setOpacity(1.0f);
	}

	switchToNewGraphData();
}

void QtGraphView::clickedInEmptySpace()
{
	std::vector<QtGraphEdge*> activeEdges;
	for (QtGraphEdge* edge: m_oldEdges)
	{
		if (edge->getIsActive())
		{
			activeEdges.push_back(edge);
		}
	}

	if (m_graph && m_graph->getTrailMode() != Graph::TRAIL_NONE)
	{
		for (QtGraphEdge* edge: activeEdges)
		{
			edge->setIsActive(false);
		}
	}
	else if (activeEdges.size() == 1)
	{
		MessageDeactivateEdge(false).dispatch();
	}
}

void QtGraphView::scrolled(int)
{
	QGraphicsView* view = getView();

	MessageScrollGraph(view->horizontalScrollBar()->value(), view->verticalScrollBar()->value())
		.dispatch();
}

void QtGraphView::resized()
{
	if (isTransitioning())
	{
		return;
	}

	doResize();
}

void QtGraphView::trailDepthChanged(int)
{
	if (m_trailDepthSlider->value() == m_trailDepthSlider->maximum())
	{
		m_trailDepthLabel->setText(QStringLiteral("inf"));
	}
	else
	{
		m_trailDepthLabel->setText(QString::number(m_trailDepthSlider->value()));
	}
}

void QtGraphView::trailDepthUpdated()
{
	if (m_oldGraph->getTrailMode() != Graph::TRAIL_NONE)
	{
		activateTrail(m_oldGraph->hasTrailOrigin());
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

void QtGraphView::clickedCustomTrail()
{
	MessageCustomTrailShow().dispatch();
}

void QtGraphView::clickedBackwardTrail()
{
	activateTrail(false);
}

void QtGraphView::clickedForwardTrail()
{
	activateTrail(true);
}

void QtGraphView::groupingUpdated(QPushButton* button)
{
	(button == m_groupFileButton ? m_groupNamespaceButton : m_groupFileButton)->setChecked(false);

	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
	if (appSettings->getGraphGrouping() != getGrouping())
	{
		appSettings->setGraphGrouping(getGrouping());
		appSettings->save();
	}

	MessageRefreshUI().noStyleReload().dispatch();
}

void QtGraphView::performScroll(QScrollBar* scrollBar, int value) const
{
	if (ApplicationSettings::getInstance()->getUseAnimations() && getView()->isVisible())
	{
		QPropertyAnimation* anim = new QPropertyAnimation(scrollBar, "value");
		anim->setDuration(300);
		anim->setStartValue(scrollBar->value());
		anim->setEndValue(value);
		anim->setEasingCurve(QEasingCurve::InOutQuad);
		anim->start();
	}
	else
	{
		scrollBar->setValue(value);
	}
}

MessageActivateTrail QtGraphView::getMessageActivateTrail(bool forward)
{
	MessageActivateTrail message(0, 0, 0, 0, false);

	QtGraphNodeData* node = dynamic_cast<QtGraphNodeData*>(m_oldActiveNode);
	if (node)
	{
		Edge::TypeMask edgeTypes;
		bool horizontalLayout = true;

		if (node->getData()->getType().isInheritable())
		{
			edgeTypes = Edge::EDGE_INHERITANCE | Edge::EDGE_TEMPLATE_SPECIALIZATION;
			horizontalLayout = false;
		}
		else if (node->getData()->getType().isCallable())
		{
			edgeTypes = Edge::EDGE_CALL | Edge::EDGE_OVERRIDE;
			horizontalLayout = true;
		}
		else if (node->getData()->getType().isFile())
		{
			edgeTypes = Edge::EDGE_INCLUDE;
			horizontalLayout = true;
		}
		else
		{
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

		return MessageActivateTrail(originId, targetId, edgeTypes, depth, horizontalLayout);
	}

	return message;
}

void QtGraphView::activateTrail(bool forward)
{
	MessageActivateTrail message = getMessageActivateTrail(forward);
	if (message.edgeTypes)
	{
		message.dispatch();
	}
}

void QtGraphView::updateTrailButtons()
{
	MessageActivateTrail message = getMessageActivateTrail(false);

	m_backwardTrailButton->setEnabled(message.edgeTypes);
	m_forwardTrailButton->setEnabled(message.edgeTypes);
	m_trailDepthLabel->setEnabled(message.edgeTypes);
	m_trailDepthSlider->setEnabled(message.edgeTypes);

	std::wstring backwardImagePath = L"graph_left.png";
	std::wstring forwardImagePath = L"graph_right.png";

	if (message.edgeTypes & Edge::EDGE_CALL)
	{
		m_backwardTrailButton->setToolTip(QStringLiteral("show caller graph"));
		m_forwardTrailButton->setToolTip(QStringLiteral("show callee graph"));
	}
	else if (message.edgeTypes & Edge::EDGE_INHERITANCE)
	{
		m_backwardTrailButton->setToolTip(QStringLiteral("show base hierarchy"));
		m_forwardTrailButton->setToolTip(QStringLiteral("show derived hierarchy"));

		backwardImagePath = L"graph_up.png";
		forwardImagePath = L"graph_down.png";
	}
	else if (message.edgeTypes & Edge::EDGE_INCLUDE)
	{
		m_backwardTrailButton->setToolTip(QStringLiteral("show including files hierarchy"));
		m_forwardTrailButton->setToolTip(QStringLiteral("show included files hierarchy"));
	}
	else
	{
		m_backwardTrailButton->setToolTip(QStringLiteral("no trail for active symbol"));
		m_forwardTrailButton->setToolTip(QStringLiteral("no trail for active symbol"));
	}

	m_forwardTrailButton->setIconPath(
		ResourcePaths::getGuiPath().concatenate(L"graph_view/images/" + forwardImagePath));
	m_backwardTrailButton->setIconPath(
		ResourcePaths::getGuiPath().concatenate(L"graph_view/images/" + backwardImagePath));
}

void QtGraphView::switchToNewGraphData()
{
	m_oldGraph = m_graph;

	for (QtGraphNode* node: m_oldNodes)
	{
		node->hide();
		node->setParentItem(nullptr);
		node->deleteLater();
	}

	for (QtGraphEdge* edge: m_oldEdges)
	{
		edge->hide();
		edge->setParentItem(nullptr);
		edge->deleteLater();
	}

	m_oldNodes = m_nodes;
	m_oldEdges = m_edges;

	m_nodes.clear();
	m_edges.clear();

	doResize();

	if (m_scrollToTop || m_restoreScroll)
	{
		updateScrollBars();
	}

	QtGraphicsView* view = getView();

	// // Manually hover the item below the mouse cursor.
	// QtGraphNode* node = view->getNodeAtCursorPosition();
	// if (node)
	// {
	// 	node->hoverEnter();
	// }

	if (m_activeNodes.size())
	{
		if (m_centerActiveNode)
		{
			centerNode(m_activeNodes.front());
		}
		m_activeNodes.clear();
	}

	if (hasNavigationFocus())
	{
		m_focusHandler.focusInitialNode();
	}

	// Repaint to make sure all artifacts are removed
	view->update();

	updateTrailButtons();

	if (m_oldGraph && m_oldGraph->getTrailMode() != Graph::TRAIL_NONE)
	{
		MessageStatus(L"Finished graph display").dispatch();
	}
}

QtGraphicsView* QtGraphView::getView() const
{
	QWidget* widget = QtViewWidgetWrapper::getWidgetOfView(this);

	QtGraphicsView* view = widget->findChild<QtGraphicsView*>(QLatin1String(""));

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

QtGraphNode* QtGraphView::createNodeRecursive(
	QGraphicsView* view,
	QtGraphNode* parentNode,
	const DummyNode* node,
	bool multipleActive,
	bool interactive)
{
	if (!node->visible)
	{
		return nullptr;
	}

	QtGraphNode* newNode = nullptr;
	if (node->isGraphNode())
	{
		newNode = new QtGraphNodeData(
			&m_focusHandler,
			node->data,
			node->name,
			node->childVisible,
			node->getQualifierNode() != nullptr,
			interactive);
	}
	else if (node->isAccessNode())
	{
		newNode = new QtGraphNodeAccess(node->accessKind);
	}
	else if (node->isExpandToggleNode())
	{
		newNode = new QtGraphNodeExpandToggle(node->isExpanded(), static_cast<int>(node->invisibleSubNodeCount));
	}
	else if (node->isBundleNode())
	{
		newNode = new QtGraphNodeBundle(
			&m_focusHandler,
			node->tokenId,
			node->getBundledNodeCount(),
			node->bundledNodeType,
			node->name,
			interactive);
	}
	else if (node->isQualifierNode())
	{
		newNode = new QtGraphNodeQualifier(node->qualifierName);
	}
	else if (node->isTextNode())
	{
		newNode = new QtGraphNodeText(node->name, node->fontSizeDiff);
	}
	else if (node->isGroupNode())
	{
		newNode = new QtGraphNodeGroup(
			&m_focusHandler,
			node->tokenId,
			node->name,
			node->groupType,
			node->interactive && interactive);
	}
	else
	{
		LOG_ERROR("DummyNode is not valid");
		return nullptr;
	}

	newNode->setPosition(node->position);
	newNode->setSize(node->size);
	newNode->setColumnSize(node->columnSize);
	newNode->setIsActive(node->active);
	newNode->setMultipleActive(multipleActive);

	if (interactive)
	{
		newNode->addComponent(std::make_shared<QtGraphNodeComponentClickable>(newNode));
	}

	view->scene()->addItem(newNode);

	if (parentNode)
	{
		newNode->setParent(parentNode);
	}
	else if (!node->isTextNode() && interactive)
	{
		newNode->addComponent(std::make_shared<QtGraphNodeComponentMoveable>(newNode));
	}

	if (node->active)
	{
		m_activeNodes.push_back(newNode);
	}

	for (unsigned int i = 0; i < node->subNodes.size(); i++)
	{
		QtGraphNode* subNode = createNodeRecursive(
			view, newNode, node->subNodes[i].get(), multipleActive, interactive);
		if (subNode)
		{
			newNode->addSubNode(subNode);
		}
	}

	newNode->updateStyle();

	return newNode;
}

QtGraphEdge* QtGraphView::createEdge(
	QGraphicsView* view,
	const DummyEdge* edge,
	std::set<Id>* visibleEdgeIds,
	Graph::TrailMode trailMode,
	QPointF pathOffset,
	bool useBezier,
	bool interactive)
{
	if (!edge->visible)
	{
		return nullptr;
	}

	QtGraphNode* owner = QtGraphNode::findNodeRecursive(m_nodes, edge->ownerId);
	QtGraphNode* target = QtGraphNode::findNodeRecursive(m_nodes, edge->targetId);

	if (owner != nullptr && target != nullptr)
	{
		QtGraphEdge* qtEdge = new QtGraphEdge(
			&m_focusHandler,
			owner,
			target,
			edge->data,
			edge->getWeight(),
			edge->active,
			interactive,
			edge->layoutHorizontal,
			edge->getDirection());

		if (trailMode != Graph::TRAIL_NONE)
		{
			std::vector<Vec4i> path = edge->path;
			for (size_t i = 0; i < path.size(); i++)
			{
				path[i].x = static_cast<int>(path[i].x - pathOffset.x());
				path[i].z = static_cast<int>(path[i].z - pathOffset.x());
				path[i].y = static_cast<int>(path[i].y - pathOffset.y());
				path[i].w = static_cast<int>(path[i].w - pathOffset.y());
			}

			for (const Vec4i& rect: path)
			{
				m_virtualNodeRects.push_back(
					QRectF(QPointF(rect.x(), rect.y()), QPointF(rect.z(), rect.w())));
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

		view->scene()->addItem(qtEdge);

		if (edge->data)
		{
			visibleEdgeIds->insert(edge->data->getId());
		}

		m_edges.push_back(qtEdge);

		return qtEdge;
	}

	return nullptr;
}

QtGraphEdge* QtGraphView::createAggregationEdge(
	QGraphicsView* view, const DummyEdge* edge, std::set<Id>* visibleEdgeIds, bool interactive)
{
	if (!edge->visible)
	{
		return nullptr;
	}

	bool allVisible = true;
	std::set<Id> aggregationIds =
		edge->data->getComponent<TokenComponentAggregation>()->getAggregationIds();
	for (Id edgeId: aggregationIds)
	{
		if (visibleEdgeIds->find(edgeId) == visibleEdgeIds->end())
		{
			allVisible = false;
			break;
		}
	}

	if (allVisible)
	{
		return nullptr;
	}

	return createEdge(view, edge, visibleEdgeIds, Graph::TRAIL_NONE, QPointF(), false, interactive);
}

QRectF QtGraphView::itemsBoundingRect(const std::list<QtGraphNode*>& items) const
{
	QRectF boundingRect;
	for (const QtGraphNode* item: items)
	{
		boundingRect |= item->sceneBoundingRect();
	}
	return boundingRect;
}

QRectF QtGraphView::getSceneRect(const std::list<QtGraphNode*>& items) const
{
	QRectF sceneRect = itemsBoundingRect(items);

	for (const QRectF& rect: m_virtualNodeRects)
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

	if (rect.width() > view->width() - 200)
	{
		rect.setWidth(view->width() - 200);
	}

	view->ensureVisibleAnimated(rect, 100, 100);
}

void QtGraphView::compareNodesRecursive(
	std::list<QtGraphNode*> newSubNodes,
	std::list<QtGraphNode*> oldSubNodes,
	std::list<QtGraphNode*>* appearingNodes,
	std::list<QtGraphNode*>* vanishingNodes,
	std::vector<std::pair<QtGraphNode*, QtGraphNode*>>* remainingNodes)
{
	for (std::list<QtGraphNode*>::iterator it = newSubNodes.begin(); it != newSubNodes.end(); it++)
	{
		bool remains = false;

		for (std::list<QtGraphNode*>::iterator it2 = oldSubNodes.begin(); it2 != oldSubNodes.end();
			 it2++)
		{
			if (((*it)->isDataNode() && (*it2)->isDataNode() &&
				 (*it)->getTokenId() == (*it2)->getTokenId()) ||
				((*it)->isAccessNode() && (*it2)->isAccessNode() &&
				 dynamic_cast<QtGraphNodeAccess*>(*it)->getAccessKind() ==
					 dynamic_cast<QtGraphNodeAccess*>(*it2)->getAccessKind()) ||
				((*it)->isExpandToggleNode() && (*it2)->isExpandToggleNode()) ||
				((*it)->isBundleNode() && (*it2)->isBundleNode() &&
				 (*it)->getTokenId() == (*it2)->getTokenId()) ||
				((*it)->isQualifierNode() && (*it2)->isQualifierNode() &&
				 (*it)->getTokenId() == (*it2)->getTokenId()) ||
				((*it)->isGroupNode() && (*it2)->isGroupNode() &&
				 (*it)->getName() == (*it2)->getName()))
			{
				remainingNodes->push_back(std::pair<QtGraphNode*, QtGraphNode*>(*it, *it2));
				compareNodesRecursive(
					(*it)->getSubNodes(),
					(*it2)->getSubNodes(),
					appearingNodes,
					vanishingNodes,
					remainingNodes);

				oldSubNodes.erase(it2);
				remains = true;
				break;
			}
		}

		if (!remains)
		{
			appearingNodes->push_back(*it);
		}
	}

	for (QtGraphNode* node: oldSubNodes)
	{
		vanishingNodes->push_back(node);
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
		for (const std::pair<QtGraphNode*, QtGraphNode*>& p: remainingNodes)
		{
			if (p.first->getPosition() != p.second->getPosition() &&
				p.first->getSize() != p.second->getSize())
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

		for (QtGraphNode* node: vanishingNodes)
		{
			QPropertyAnimation* anim = new QPropertyAnimation(node, QByteArrayLiteral("opacity"));
			anim->setDuration(300);
			anim->setStartValue(1.0f);
			anim->setEndValue(0.0f);

			vanish->addAnimation(anim);
		}

		for (QtGraphEdge* edge: m_oldEdges)
		{
			QPropertyAnimation* anim = new QPropertyAnimation(edge, QByteArrayLiteral("opacity"));
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

		for (std::pair<QtGraphNode*, QtGraphNode*> p: remainingNodes)
		{
			QtGraphNode* newNode = p.first;
			QtGraphNode* oldNode = p.second;

			QPropertyAnimation* anim = new QPropertyAnimation(oldNode, QByteArrayLiteral("pos"));
			anim->setDuration(300);
			anim->setStartValue(oldNode->pos());
			anim->setEndValue(newNode->pos());

			remain->addAnimation(anim);

			connect(anim, &QPropertyAnimation::finished, newNode, &QtGraphNode::showNode);
			connect(anim, &QPropertyAnimation::finished, oldNode, &QtGraphNode::hideNode);
			newNode->hide();

			anim = new QPropertyAnimation(oldNode, QByteArrayLiteral("size"));
			anim->setDuration(300);
			anim->setStartValue(oldNode->size());
			anim->setEndValue(newNode->size());

			remain->addAnimation(anim);

			if (newNode->isAccessNode() && newNode->getSubNodes().size() == 0 &&
				oldNode->getSubNodes().size() > 0)
			{
				dynamic_cast<QtGraphNodeAccess*>(oldNode)->hideLabel();
			}
		}

		QPropertyAnimation* anim = new QPropertyAnimation(view, QByteArrayLiteral("sceneRect"));
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

		for (QtGraphNode* node: appearingNodes)
		{
			QPropertyAnimation* anim = new QPropertyAnimation(node, "opacity");
			anim->setDuration(300);
			anim->setStartValue(0.0f);
			anim->setEndValue(1.0f);

			appear->addAnimation(anim);

			connect(anim, &QPropertyAnimation::finished, node, &QtGraphNode::blendIn);
			node->blendOut();
		}

		for (QtGraphEdge* edge: m_edges)
		{
			QPropertyAnimation* anim = new QPropertyAnimation(edge, "opacity");
			anim->setDuration(150);
			anim->setStartValue(0.0f);
			anim->setEndValue(1.0f);

			appear->addAnimation(anim);

			edge->setOpacity(0.0f);
		}

		m_transition->addAnimation(appear);
	}

	connect(
		m_transition.get(), &QPropertyAnimation::finished, this, &QtGraphView::finishedTransition);
	m_transition->start();
}

bool QtGraphView::isTransitioning() const
{
	return m_transition && m_transition->currentTime() < m_transition->totalDuration();
}
