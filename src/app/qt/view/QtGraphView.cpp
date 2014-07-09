#include "QtGraphView.h"

#include "qboxlayout.h"
#include "qgraphicsitem.h"
#include "qgraphicsproxywidget.h"
#include "qgraphicsscene.h"
#include "qgraphicsview.h"
#include "qpushbutton.h"

#include "qt/utility/utilityQt.h"

#include "qt/QtWidgetWrapper.h"
#include "qt/view/graphElements/QtGraphEdge.h"
#include "qt/view/graphElements/QtGraphNode.h"
#include "qt/view/graphElements/QtGraphNodeMovable.h"

QtGraphView::QtGraphView(ViewLayout* viewLayout)
	: GraphView(viewLayout)
	, m_rebuildGraph(std::bind(&QtGraphView::doRebuildGraph, this, std::placeholders::_1, std::placeholders::_2))
	, m_clear(std::bind(&QtGraphView::doClear, this))
{
}

QtGraphView::~QtGraphView()
{
}

void QtGraphView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtWidgetWrapper>(std::make_shared<QFrame>()));
}

void QtGraphView::initGui()
{
	QWidget* widget = QtWidgetWrapper::getWidgetOfView(this);
	utility::setWidgetBackgroundColor(widget, Colori(100, 20, 200, 255));

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setSpacing(3);
	layout->setContentsMargins(3, 3, 3, 3);
	widget->setLayout(layout);

	QGraphicsScene* scene = new QGraphicsScene(widget);

	QGraphicsView* view = new QGraphicsView(widget);
	view->setScene(scene);

	widget->layout()->addWidget(view);
}

void QtGraphView::rebuildGraph(const std::vector<DummyNode>& nodes, const std::vector<DummyEdge>& edges)
{
	m_rebuildGraph(nodes, edges);
}

void QtGraphView::clear()
{
	m_clear();
}

QGraphicsView* QtGraphView::getView()
{
	QWidget* widget = QtWidgetWrapper::getWidgetOfView(this);

	QLayout* layout = widget->layout();

	QObjectList children = widget->children();

	return widget->findChild<QGraphicsView*>("");
}

void QtGraphView::doRebuildGraph(const std::vector<DummyNode>& nodes, const std::vector<DummyEdge>& edges)
{
	QGraphicsView* view = getView();

	if (view != NULL)
	{
		std::map<Id, std::weak_ptr<GraphNode>> weakNodes; // used when creating the edges
		std::list<std::shared_ptr<GraphNode>> newNodes; // temporary stores all nodes (existing and newly created) needed in the new graph
														// this is a relatively easy and cheap way to save existing nodes that are still needed

		// create nodes (or find existing nodes for re-use)
		for(unsigned int i = 0; i < nodes.size(); i++)
		{
			std::shared_ptr<GraphNode> newNode = findOrCreateNode(view, nodes[i]);
			newNodes.push_back(newNode);
			weakNodes[nodes[i].tokenId] = newNode;
		}

		doClear();
		m_nodes = newNodes;

		// create edges
		for(unsigned int i = 0; i < edges.size(); i++)
		{
			if (weakNodes.find(edges[i].ownerId) != weakNodes.end() && weakNodes.find(edges[i].targetId) != weakNodes.end())
			{
				if (weakNodes[edges[i].ownerId].expired() == false && weakNodes[edges[i].targetId].expired() == false)
				{
					std::shared_ptr<QtGraphEdge> newEdge = std::make_shared<QtGraphEdge>(weakNodes[edges[i].ownerId], weakNodes[edges[i].targetId]);
					view->scene()->addItem(newEdge.get());
					if(weakNodes[edges[i].ownerId].lock()->addOutEdge(newEdge))
					{
						weakNodes[edges[i].targetId].lock()->addInEdge(newEdge);
						m_edges.push_back(newEdge);
					}
				}
			}
		}
	}
	else
	{
		LOG_WARNING("Failed to get QGraphicsView");
	}
}

void QtGraphView::doClear()
{
	m_nodes.clear();
	m_edges.clear();
}

std::shared_ptr<GraphNode> QtGraphView::findOrCreateNode(QGraphicsView* view, const DummyNode& node)
{
	std::shared_ptr<GraphNode> result;

	result = findNode(node);

	if(result == NULL)
	{
		result = createNode(view, node);
	}

	return result;
}

std::shared_ptr<GraphNode> QtGraphView::findNode(const DummyNode& node)
{
	std::list<std::shared_ptr<GraphNode>>::iterator it = m_nodes.begin();

	for(it; it != m_nodes.end(); it++)
	{
		if((*it)->getTokenId() == node.tokenId)
		{
			return *it;
		}
	}

	return NULL;
}

std::shared_ptr<GraphNode> QtGraphView::createNode(QGraphicsView* view, const DummyNode& node)
{
	if(view != NULL)
	{
		std::shared_ptr<QtGraphNodeMovable> newNode = std::make_shared<QtGraphNodeMovable>(node.position, node.name, node.tokenId);
		view->scene()->addItem(newNode.get());
		m_nodes.push_back(newNode);

		// create debug sub node
		std::shared_ptr<QtGraphNode> subNode = std::make_shared<QtGraphNode>(Vec2i(10, 30), node.name + " member", 666);
		subNode->setParentItem(newNode.get());
		view->scene()->addItem(subNode.get());
		newNode->addSubNode(subNode);

		subNode->setRect(0, 0, 80, 20);

		return newNode;
	}
	else
	{
		LOG_WARNING("Received pointer to QGraphicsView was NULL. No node created.");
	}
}
