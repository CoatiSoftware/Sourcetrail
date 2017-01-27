#include "qt/view/QtGraphView.h"

#include <QBoxLayout>
#include <QFrame>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QScrollBar>
#include <QSequentialAnimationGroup>

#include "component/controller/helper/DummyEdge.h"
#include "component/controller/helper/DummyNode.h"
#include "component/controller/helper/GraphPostprocessor.h"
#include "component/view/GraphViewStyle.h"
#include "settings/ColorScheme.h"
#include "settings/ApplicationSettings.h"
#include "utility/messaging/type/MessageDeactivateEdge.h"
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

QtGraphView::QtGraphView(ViewLayout* viewLayout)
	: GraphView(viewLayout)
	, m_rebuildGraphFunctor(
		std::bind(&QtGraphView::doRebuildGraph, this,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4))
	, m_clearFunctor(std::bind(&QtGraphView::doClear, this))
	, m_resizeFunctor(std::bind(&QtGraphView::doResize, this))
	, m_refreshFunctor(std::bind(&QtGraphView::doRefreshView, this))
	, m_focusInFunctor(std::bind(&QtGraphView::doFocusIn, this, std::placeholders::_1))
	, m_focusOutFunctor(std::bind(&QtGraphView::doFocusOut, this, std::placeholders::_1))
	, m_zoomInButton()
	, m_zoomOutButton()
	, m_zoomInButtonSpeed(20.0f)
	, m_zoomOutButtonSpeed(-20.0f)
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

	m_zoomInButton = new QPushButton(widget);
	m_zoomInButton->setObjectName("zoom_in_button");
	m_zoomInButton->setText("+");
	m_zoomInButton->setGeometry(QRect(5, 5, 25, 25));
	m_zoomInButton->setAutoRepeat(true);
	m_zoomInButton->setToolTip("Zoom in (Shift + Mousewheel forward)");
	connect(m_zoomInButton, SIGNAL(pressed()), this, SLOT(zoomInPressed()));

	m_zoomOutButton = new QPushButton(widget);
	m_zoomOutButton->setObjectName("zoom_out_button");
	m_zoomOutButton->setText("-");
	m_zoomOutButton->setGeometry(QRect(5, 34, 25, 25));
	m_zoomOutButton->setAutoRepeat(true);
	m_zoomOutButton->setToolTip("Zoom out (Shift + Mousewheel back)");
	connect(m_zoomOutButton, SIGNAL(pressed()), this, SLOT(zoomOutPressed()));

	connect(view, SIGNAL(emptySpaceClicked()), this, SLOT(clickedInEmptySpace()));

	m_scrollSpeedChangeListenerHorizontal.setScrollBar(view->horizontalScrollBar());
	m_scrollSpeedChangeListenerVertical.setScrollBar(view->verticalScrollBar());

	doRefreshView();
}

void QtGraphView::refreshView()
{
	m_refreshFunctor();
}

void QtGraphView::rebuildGraph(
	std::shared_ptr<Graph> graph,
	const std::vector<std::shared_ptr<DummyNode>>& nodes,
	const std::vector<std::shared_ptr<DummyEdge>>& edges,
	const GraphParams params
){
	m_rebuildGraphFunctor(graph, nodes, edges, params);
}

void QtGraphView::clear()
{
	m_clearFunctor();
}

void QtGraphView::resizeView()
{
	m_resizeFunctor();
}

Vec2i QtGraphView::getViewSize() const
{
	QtGraphicsView* view = getView();

	float zoomFactor = view->getZoomFactor();
	return Vec2i(view->width() / zoomFactor - 80, view->height() / zoomFactor - 80);
}

void QtGraphView::centerScrollBars()
{
	QGraphicsView* view = getView();

	QScrollBar* hb = view->horizontalScrollBar();
	QScrollBar* vb = view->verticalScrollBar();

	hb->setValue((hb->minimum() + hb->maximum()) / 2);
	vb->setValue((vb->minimum() + vb->maximum()) / 2);
}

void QtGraphView::finishedTransition()
{
	QGraphicsView* view = getView();
	view->setInteractive(true);

	switchToNewGraphData();
}

void QtGraphView::clickedInEmptySpace()
{
	size_t activeEdgeCount = 0;
	for (std::shared_ptr<QtGraphEdge> edge : m_oldEdges)
	{
		if (edge->getIsActive())
		{
			activeEdgeCount++;
		}
	}

	if (activeEdgeCount == 1)
	{
		MessageDeactivateEdge().dispatch();
	}
}

void QtGraphView::zoomInPressed()
{
	zoom(m_zoomInButtonSpeed);
}

void QtGraphView::zoomOutPressed()
{
	zoom(m_zoomOutButtonSpeed);
}

void QtGraphView::switchToNewGraphData()
{
	m_oldGraph = m_graph;

	for (const std::shared_ptr<QtGraphNode>& node : m_oldNodes)
	{
		node->hide();
	}

	for (const std::shared_ptr<QtGraphEdge>& edge : m_oldEdges)
	{
		edge->hide();
	}

	m_oldNodes = m_nodes;
	m_oldEdges = m_edges;

	m_nodes.clear();
	m_edges.clear();

	doResize();

	// Manually hover the item below the mouse cursor.
	QtGraphicsView* view = getView();
	QtGraphNode* node = view->getNodeAtCursorPosition();
	if (node)
	{
		node->hoverEnter();
	}

	if (m_activeNode)
	{
		Vec2i pos = m_activeNode->getPosition();
		Vec2i size = m_activeNode->getSize();

		QRectF rect(pos.x, pos.y, size.x, size.y);

		if (rect.height() > view->height() - 200)
		{
			rect.setHeight(view->height() - 200);
		}

		view->ensureVisibleAnimated(rect, 100, 100);
		m_activeNode.reset();
	}

	// Repaint to make sure all artifacts are removed
	view->update();
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

void QtGraphView::doRebuildGraph(
	std::shared_ptr<Graph> graph,
	const std::vector<std::shared_ptr<DummyNode>>& nodes,
	const std::vector<std::shared_ptr<DummyEdge>>& edges,
	const GraphParams params
){
	if (m_transition && m_transition->currentTime() < m_transition->totalDuration())
	{
		m_transition->stop();
		finishedTransition();
	}

	QGraphicsView* view = getView();

	size_t activeNodeCount = 0;
	for (unsigned int i = 0; i < nodes.size(); i++)
	{
		activeNodeCount += nodes[i]->getActiveSubNodeCount();
	}

	m_nodes.clear();
	m_activeNode.reset();
	for (unsigned int i = 0; i < nodes.size(); i++)
	{
		std::shared_ptr<QtGraphNode> node = createNodeRecursive(view, NULL, nodes[i].get(), activeNodeCount > 1);
		if (node)
		{
			m_nodes.push_back(node);
		}
	}

	QPointF center = itemsBoundingRect(m_nodes).center();
	Vec2i o = GraphPostprocessor::alignOnRaster(Vec2i(center.x(), center.y()));
	QPointF offset = QPointF(o.x, o.y);
	m_sceneRectOffset = offset - center;

	for (const std::shared_ptr<QtGraphNode>& node : m_nodes)
	{
		node->setPos(node->pos() - offset);
	}

	m_edges.clear();

	std::set<Id> visibleEdgeIds;
	for (std::shared_ptr<DummyEdge> edge : edges)
	{
		if (!edge->data || !edge->data->isType(Edge::EDGE_AGGREGATION))
		{
			createEdge(view, edge.get(), &visibleEdgeIds);
		}
	}
	for (std::shared_ptr<DummyEdge> edge : edges)
	{
		if (edge->data && edge->data->isType(Edge::EDGE_AGGREGATION))
		{
			createAggregationEdge(view, edge.get(), &visibleEdgeIds);
		}
	}


	if (graph)
	{
		m_graph = graph;
	}

	if (!params.centerActiveNode)
	{
		m_activeNode.reset();
	}

	if (params.animatedTransition && ApplicationSettings::getInstance()->getUseAnimations())
	{
		createTransition();
	}
	else
	{
		switchToNewGraphData();
	}
}

void QtGraphView::doClear()
{
	m_nodes.clear();
	m_edges.clear();

	m_oldNodes.clear();
	m_oldEdges.clear();

	m_graph.reset();
	m_oldGraph.reset();
}

void QtGraphView::doResize()
{
	getView()->setSceneRect(getSceneRect(m_oldNodes));
}

void QtGraphView::doRefreshView()
{
	doResize();

	QtGraphicsView* view = getView();

	std::string css = utility::getStyleSheet(ResourcePaths::getGuiPath() + "graph_view/graph_view.css");
	view->setStyleSheet(css.c_str());

	view->setAppZoomFactor(GraphViewStyle::getZoomFactor());
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
		newNode = std::make_shared<QtGraphNodeBundle>(node->tokenId, node->getBundledNodeCount(), node->name);
	}
	else if (node->isQualifierNode())
	{
		newNode = std::make_shared<QtGraphNodeQualifier>(node->qualifierName);
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
	else
	{
		newNode->addComponent(std::make_shared<QtGraphNodeComponentMoveable>(newNode));
	}

	if (node->active && !m_activeNode)
	{
		m_activeNode = newNode;
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
	QGraphicsView* view, const DummyEdge* edge, std::set<Id>* visibleEdgeIds)
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
			std::make_shared<QtGraphEdge>(owner, target, edge->data, edge->getWeight(), edge->active, edge->getDirection());

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

	return createEdge(view, edge, visibleEdgeIds);
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
	return itemsBoundingRect(items).adjusted(-25, -25, 25, 25).translated(m_sceneRectOffset);
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
				((*it)->isBundleNode() && (*it2)->isBundleNode() && (*it)->getTokenId() == (*it2)->getTokenId()))
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

		for (std::shared_ptr<QtGraphEdge> edge : m_oldEdges)
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

			connect(anim, SIGNAL(finished()), newNode, SLOT(showNode()));
			connect(anim, SIGNAL(finished()), oldNode, SLOT(hideNode()));
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

		if (!remainingNodes.size())
		{
			connect(anim, SIGNAL(finished()), this, SLOT(centerScrollBars()));
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

			connect(anim, SIGNAL(finished()), node, SLOT(blendIn()));
			node->blendOut();
		}

		for (std::shared_ptr<QtGraphEdge> edge : m_edges)
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

	connect(m_transition.get(), SIGNAL(finished()), this, SLOT(finishedTransition()));
	m_transition->start();
}

void QtGraphView::focusTokenIds(const std::vector<Id>& focusedTokenIds)
{
	m_focusInFunctor(focusedTokenIds);
}

void QtGraphView::doFocusIn(const std::vector<Id>& tokenIds)
{
	for (const Id& tokenId : tokenIds)
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
}

void QtGraphView::defocusTokenIds(const std::vector<Id>& defocusedTokenIds)
{
	m_focusOutFunctor(defocusedTokenIds);
}

void QtGraphView::doFocusOut(const std::vector<Id>& tokenIds)
{
	for (const Id& tokenId : tokenIds)
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
}

void QtGraphView::zoom(const float delta)
{
	QtGraphicsView* view = getView();
	if (view != NULL)
	{
		view->updateZoom(delta);
	}
}
