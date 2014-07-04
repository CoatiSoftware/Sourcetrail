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

std::weak_ptr<GraphNode> QtGraphView::addNode(const Vec2i& position, const std::string& name)
{
	QGraphicsView* view = getView();

	if (view != NULL)
	{
		std::shared_ptr<QtGraphNode> node = std::make_shared<QtGraphNode>(position, name);
		view->scene()->addItem(node.get());
		node->setFlag(QGraphicsItem::ItemIsMovable);

		m_nodes.push_back(node);

		return node;
	}
	else
	{
		LOG_WARNING("Unable to retrieve view from widget");

		return std::weak_ptr<QtGraphNode>();
	}
}

void QtGraphView::addEdge(const std::weak_ptr<GraphNode>& owner, const std::weak_ptr<GraphNode>& target)
{
	QGraphicsView* view = getView();

	if (view != NULL)
	{
		std::shared_ptr<GraphNode> o = owner.lock();
		std::shared_ptr<GraphNode> t = target.lock();

		if (o != NULL && t != NULL)
		{
			std::shared_ptr<QtGraphEdge> edge = std::make_shared<QtGraphEdge>(owner, target);
			view->scene()->addItem(edge.get());

			m_edges.push_back(edge);

			o->addOutEdge(edge);
			t->addInEdge(edge);
		}
		else
		{
			LOG_WARNING("Either the owner or target node could not be locked, make sure they still exist!");
		}
	}
	else
	{
		LOG_WARNING("Unable to retrieve view from widget");
	}
}

QGraphicsView* QtGraphView::getView()
{
	QWidget* widget = QtWidgetWrapper::getWidgetOfView(this);

	QLayout* layout = widget->layout();

	QObjectList children = widget->children();

	return widget->findChild<QGraphicsView*>("");
}
