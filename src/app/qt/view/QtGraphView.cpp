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
		std::bind(
			&QtGraphView::doRebuildGraph, this,
			std::placeholders::_1, std::placeholders::_2,
			std::placeholders::_3, std::placeholders::_4))
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
	std::vector<Id> activeTokenIds,
	const std::vector<DummyNode>& nodes,
	const std::vector<DummyEdge>& edges
){
	m_rebuildGraphFunctor(graph, activeTokenIds, nodes, edges);
}

void QtGraphView::clear()
{
	m_clearFunctor();
}

QGraphicsView* QtGraphView::getView()
{
	QWidget* widget = QtViewWidgetWrapper::getWidgetOfView(this);

	QObjectList children = widget->children();

	return widget->findChild<QGraphicsView*>("");
}

void QtGraphView::doRebuildGraph(
	std::shared_ptr<Graph> graph,
	std::vector<Id> activeTokenIds,
	const std::vector<DummyNode>& nodes,
	const std::vector<DummyEdge>& edges
){
	m_activeTokenIds = activeTokenIds;
	QGraphicsView* view = getView();

	if (view != NULL)
	{
		// Temporary stores all nodes (existing and newly created) needed in the new graph
		// this is a relatively easy and cheap way to save existing nodes that are still needed
		std::list<std::shared_ptr<QtGraphNode>> newNodes;

		// create nodes (or find existing nodes for re-use)
		for (unsigned int i = 0; i < nodes.size(); i++)
		{
			std::shared_ptr<QtGraphNode> newNode = createNodeRecursive(view, NULL, nodes[i]);
			newNode->setPosition(nodes[i].position);
			newNodes.push_back(newNode);
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

		// hide node content by default, must be done after all edges are created to keep subnodes with connections visible
		for (const std::shared_ptr<QtGraphNode>& node : m_nodes)
		{
			node->hideContent();
		}
	}
	else
	{
		LOG_WARNING("Failed to get QGraphicsView");
	}

	m_graph = graph;
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
	if (view != NULL)
	{
		std::shared_ptr<QtGraphNode> newNode;
		if (node.data)
		{
			newNode = std::make_shared<QtGraphNode>(node.data);
		}
		else
		{
			newNode = std::make_shared<QtGraphNodeAccess>(node.accessType);
		}

		newNode->addComponent(std::make_shared<QtGraphNodeComponentClickable>(newNode));

		view->scene()->addItem(newNode.get());

		if (node.data && isActiveTokenId(node.data->getId()))
		{
			newNode->setIsActive(true);
		}

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
			newNode->addSubNode(subNode);
		}

		return newNode;
	}
	else
	{
		LOG_WARNING("Received pointer to QGraphicsView was NULL. No node created.");
		return std::shared_ptr<QtGraphNode>();
	}
}

std::shared_ptr<QtGraphEdge> QtGraphView::createEdge(QGraphicsView* view, const DummyEdge& edge)
{
	if (view != NULL)
	{
		std::shared_ptr<QtGraphNode> owner = findNodeRecursive(m_nodes, edge.ownerId);
		std::shared_ptr<QtGraphNode> target = findNodeRecursive(m_nodes, edge.targetId);

		if (owner != NULL && target != NULL)
		{
			std::shared_ptr<QtGraphEdge> qtEdge = std::make_shared<QtGraphEdge>(owner, target, edge.data);
			owner->addOutEdge(qtEdge);
			target->addInEdge(qtEdge);
			view->scene()->addItem(qtEdge.get());

			if (isActiveTokenId(edge.data->getId()))
			{
				qtEdge->setIsActive(true);
			}

			return qtEdge;
		}
		else
		{
			LOG_WARNING("Couldn't find owner or target node.");
			return std::shared_ptr<QtGraphEdge>();
		}
	}
	else
	{
		LOG_WARNING("Received pointer to QGraphicsView was NULL. No node created.");
		return std::shared_ptr<QtGraphEdge>();
	}
}

bool QtGraphView::isActiveTokenId(Id tokenId) const
{
	return find(m_activeTokenIds.begin(), m_activeTokenIds.end(), tokenId) != m_activeTokenIds.end();
}
