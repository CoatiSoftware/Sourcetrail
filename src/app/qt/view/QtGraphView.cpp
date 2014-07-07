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
	doClear();

	QGraphicsView* view = getView();

	if (view != NULL)
	{
		for(unsigned int i = 0; i < nodes.size(); i++)
		{
			std::shared_ptr<GraphNode> newNode = findOrCreateNode(view, nodes[i]);
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
		std::shared_ptr<QtGraphNode> newNode = std::make_shared<QtGraphNode>(node.position, node.name, node.tokenId);
		view->scene()->addItem(newNode.get());
		m_nodes.push_back(newNode);

		return newNode;
	}
	else
	{
		LOG_WARNING("Received pointer to QGraphicsView was NULL. No node created.");
	}
}
