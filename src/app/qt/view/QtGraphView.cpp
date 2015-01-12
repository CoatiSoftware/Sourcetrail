#include "QtGraphView.h"

#include <QBoxLayout>
#include <QFrame>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "qt/utility/utilityQt.h"

#include "qt/view/QtViewWidgetWrapper.h"
#include "qt/view/graphElements/QtGraphEdge.h"
#include "qt/view/graphElements/QtGraphNode.h"
#include "qt/view/graphElements/QtGraphNodeAccess.h"
#include "qt/view/graphElements/nodeComponents/QtGraphNodeComponentClickable.h"
#include "qt/view/graphElements/nodeComponents/QtGraphNodeComponentMoveable.h"

QtGraphView::QtGraphView(ViewLayout* viewLayout)
	: GraphView(viewLayout)
	, m_rebuildGraphFunctor(
		std::bind(&QtGraphView::doRebuildGraph, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
	, m_clearFunctor(std::bind(&QtGraphView::doClear, this))
{
}

QtGraphView::~QtGraphView()
{
}

void QtGraphView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(std::make_shared<QFrame>()));
}

void QtGraphView::initView()
{
	QWidget* widget = QtViewWidgetWrapper::getWidgetOfView(this);
	utility::setWidgetBackgroundColor(widget, Colori(255, 255, 255, 255));

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	widget->setLayout(layout);

	QGraphicsScene* scene = new QGraphicsScene(widget);
	QGraphicsView* view = new QGraphicsView(widget);
	view->setScene(scene);

	widget->layout()->addWidget(view);
}

void QtGraphView::refreshView()
{
}

void QtGraphView::rebuildGraph(
	std::shared_ptr<Graph> graph,
	const std::vector<DummyNode>& nodes,
	const std::vector<DummyEdge>& edges
){
	m_rebuildGraphFunctor(graph, nodes, edges);
}

void QtGraphView::clear()
{
	m_clearFunctor();
}

GraphView::Metrics QtGraphView::getViewMetrics() const
{
	GraphView::Metrics metrics;

	QGraphicsView* view = getView();
	if (view == NULL)
	{
		return metrics;
	}

	metrics.width = view->width();
	metrics.height = view->height();

	metrics.typeNameCharWidth = QFontMetrics(QtGraphNode::getFontForNodeType(Node::NODE_CLASS)).width("QtGraphNode") / 11.0f;
	metrics.variableNameCharWidth = QFontMetrics(QtGraphNode::getFontForNodeType(Node::NODE_FIELD)).width("QtGraphNode") / 11.0f;
	metrics.functionNameCharWidth = QFontMetrics(QtGraphNode::getFontForNodeType(Node::NODE_METHOD)).width("QtGraphNode") / 11.0f;

	return metrics;
}

QGraphicsView* QtGraphView::getView() const
{
	QWidget* widget = QtViewWidgetWrapper::getWidgetOfView(this);

	return widget->findChild<QGraphicsView*>("");
}

void QtGraphView::doRebuildGraph(
	std::shared_ptr<Graph> graph,
	const std::vector<DummyNode>& nodes,
	const std::vector<DummyEdge>& edges
){
	QGraphicsView* view = getView();

	if (view == NULL)
	{
		LOG_WARNING("Failed to get QGraphicsView");
		return;
	}

	// Temporary stores all nodes (existing and newly created) needed in the new graph
	// this is a relatively easy and cheap way to save existing nodes that are still needed
	std::list<std::shared_ptr<QtGraphNode>> newNodes;

	// create nodes (or find existing nodes for re-use)
	for (unsigned int i = 0; i < nodes.size(); i++)
	{
		std::shared_ptr<QtGraphNode> node = createNodeRecursive(view, NULL, nodes[i]);
		if (node)
		{
			newNodes.push_back(node);
		}
	}

	doClear();
	m_nodes = newNodes;

	for (unsigned int i = 0; i < edges.size(); i++)
	{
		std::shared_ptr<QtGraphEdge> edge = createEdge(view, edges[i]);
		if (edge != NULL)
		{
			m_edges.push_back(edge);
		}
	}

	if (graph)
	{
		m_graph = graph;
	}

	// Manually hover the items below the mouse cursor.
	view->scene()->setSceneRect(view->scene()->itemsBoundingRect());
	QPointF point = view->mapToScene(view->mapFromGlobal(QCursor::pos()));
	QGraphicsItem* item = view->scene()->itemAt(point, QTransform());
	if (item)
	{
		QtGraphNode* node = dynamic_cast<QtGraphNode*>(item->parentItem());
		if (node)
		{
			node->hoverEnter();
		}
	}
}

void QtGraphView::doClear()
{
	m_nodes.clear();
	m_edges.clear();
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
	QGraphicsView* view, std::shared_ptr<QtGraphNode> parentNode, const DummyNode& node
){
	if (!node.visible)
	{
		return NULL;
	}

	std::shared_ptr<QtGraphNode> newNode;
	if (node.data)
	{
		newNode = std::make_shared<QtGraphNode>(node.data);
	}
	else
	{
		newNode = std::make_shared<QtGraphNodeAccess>(node.accessType, node.isExpanded(), node.invisibleSubNodeCount);
	}

	newNode->setPosition(node.position);
	newNode->setSize(node.size);
	newNode->setIsActive(node.active);

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

	for (unsigned int i = 0; i < node.subNodes.size(); i++)
	{
		std::shared_ptr<QtGraphNode> subNode = createNodeRecursive(view, newNode, node.subNodes[i]);
		if (subNode)
		{
			newNode->addSubNode(subNode);
		}
	}

	newNode->setStyle();

	return newNode;
}

std::shared_ptr<QtGraphEdge> QtGraphView::createEdge(QGraphicsView* view, const DummyEdge& edge)
{
	std::shared_ptr<QtGraphNode> owner = findNodeRecursive(m_nodes, edge.ownerId);
	std::shared_ptr<QtGraphNode> target = findNodeRecursive(m_nodes, edge.targetId);

	if (owner != NULL && target != NULL)
	{
		std::shared_ptr<QtGraphEdge> qtEdge = std::make_shared<QtGraphEdge>(owner, target, edge.data);
		qtEdge->setIsActive(edge.active);

		bool added = owner->addOutEdge(qtEdge) | target->addInEdge(qtEdge);
		if (!added)
		{
			return NULL;
		}

		view->scene()->addItem(qtEdge.get());

		return qtEdge;
	}
	else
	{
		LOG_WARNING_STREAM(<< "Couldn't find owner or target node for edge: " << edge.data->getName());
		return NULL;
	}
}
