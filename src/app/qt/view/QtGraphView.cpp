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
#include "qt/view/graphElements/QtGraphNodeMouseMovable.h"

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
			newNode->setPosition(nodes[i].position);
			newNodes.push_back(newNode);
			weakNodes[nodes[i].tokenId] = newNode;
		}

		doClear();
		m_nodes = newNodes;

		for(unsigned int i = 0; i < edges.size(); i++)
		{
			std::shared_ptr<GraphEdge> edge = createEdge(view, edges[i]);
			if(edge != NULL)
			{
				m_edges.push_back(edge);
			}
		}

		// create edges
		/*for(unsigned int i = 0; i < edges.size(); i++)
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
		}*/
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

	result = findNode(node.tokenId);

	if(result == NULL)
	{
		result = createNode(view, node);
	}

	return result;
}

std::shared_ptr<GraphNode> QtGraphView::findNode(const Id id)
{
	std::list<std::shared_ptr<GraphNode>>::iterator it = m_nodes.begin();

	for(it; it != m_nodes.end(); it++)
	{
		if((*it)->getTokenId() == id)
		{
			return *it;
		}
		else
		{
			std::shared_ptr<GraphNode> result = findSubNode(*it, id);
			if(result != NULL)
			{
				return result;
			}
		}
	}

	return std::shared_ptr<GraphNode>();
}

std::shared_ptr<GraphNode> QtGraphView::findSubNode(const std::shared_ptr<GraphNode> node, const Id id)
{
	std::list<std::shared_ptr<GraphNode>> subNodes = node->getSubNodes();

	std::list<std::shared_ptr<GraphNode>>::iterator it = subNodes.begin();
	for(it; it != subNodes.end(); it++)
	{
		if((*it)->getTokenId() == id)
		{
			return *it;
		}
		else
		{
			std::shared_ptr<GraphNode> result = findSubNode(*it, id);
			if(result != NULL)
			{
				return result;
			}
		}
	}

	return std::shared_ptr<GraphNode>();
}

std::shared_ptr<GraphNode> QtGraphView::createNode(QGraphicsView* view, const DummyNode& node)
{
	if(view != NULL)
	{
		std::shared_ptr<QtGraphNodeMouseMovable> newNode = std::make_shared<QtGraphNodeMouseMovable>(Vec2i(0, 0), node.name, node.tokenId);
		view->scene()->addItem(newNode.get());
		m_nodes.push_back(newNode);

		for(unsigned int i = 0; i < node.subNodes.size(); i++)
		{
			std::shared_ptr<QtGraphNode> subNode = createSubNode(view, node.subNodes[i]);
			subNode->setParentItem(newNode.get());
			newNode->addSubNode(subNode);
			subNode->setRect(0, 0, 80, 20);
			subNode->moveBy(10, (i+1)*30);
		}

		newNode->setRect(0, 0, 100, 40 + (node.subNodes.size() * 30));

		return newNode;
	}
	else
	{
		LOG_WARNING("Received pointer to QGraphicsView was NULL. No node created.");
		return std::shared_ptr<QtGraphNode>();
	}
}

std::shared_ptr<QtGraphNode> QtGraphView::createSubNode(QGraphicsView* view, const DummyNode& node)
{
	if(view != NULL)
	{
		std::shared_ptr<QtGraphNode> newNode = std::make_shared<QtGraphNode>(Vec2i(0, 0), node.name, node.tokenId);
		view->scene()->addItem(newNode.get());

		for(unsigned int i = 0; i < node.subNodes.size(); i++)
		{
			std::shared_ptr<QtGraphNode> subNode = createSubNode(view, node.subNodes[i]);
			subNode->setParentItem(newNode.get());
			newNode->addSubNode(subNode);
			subNode->setRect(0, 0, 80, 20);
			subNode->moveBy(10, (i+1)*30);
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
	if(view != NULL)
	{
		std::shared_ptr<GraphNode> owner = findNode(edge.ownerId);
		std::shared_ptr<GraphNode> target = findNode(edge.targetId);

		if(owner != NULL && target != NULL)
		{
			std::shared_ptr<QtGraphEdge> edge = std::make_shared<QtGraphEdge>(owner, target);
			owner->addOutEdge(edge);
			target->addInEdge(edge);
			view->scene()->addItem(edge.get());

			return edge;
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
