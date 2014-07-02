#include "QtGraphView.h"

#include "qboxlayout.h"
#include "qgraphicsitem.h"
#include "qgraphicsproxywidget.h"
#include "qgraphicsscene.h"
#include "qgraphicsview.h"
#include "qpushbutton.h"

#include "qt/utility/utilityQt.h"

#include "qt/QtWidgetWrapper.h"
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

void QtGraphView::addNode(const Vec2i& position, const std::string& name)
{
	QWidget* widget = QtWidgetWrapper::getWidgetOfView(this);

	QLayout* layout = widget->layout();

	QObjectList children = widget->children();

	QGraphicsView* view = widget->findChild<QGraphicsView*>("");

	if(view != NULL)
	{
		QtGraphNode* node = new QtGraphNode(position, name);
		view->scene()->addItem(node);
		node->setFlag(QGraphicsItem::ItemIsMovable);
	}
	else
	{
		LOG_WARNING("Unable to retrieve view from widget");
	}
}
