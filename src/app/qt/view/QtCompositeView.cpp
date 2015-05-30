#include "qt/view/QtCompositeView.h"

#include <QBoxLayout>

#include "qt/utility/utilityQt.h"
#include "qt/view/QtViewWidgetWrapper.h"

QtCompositeView::QtCompositeView(ViewLayout* viewLayout, CompositeDirection direction)
	: CompositeView(viewLayout, direction)
{
	QBoxLayout* layout;
	if (getDirection() == CompositeView::DIRECTION_HORIZONTAL)
	{
		layout = new QHBoxLayout();
	}
	else
	{
		layout = new QVBoxLayout();
	}

	layout->setSpacing(5);
	layout->setContentsMargins(8, 8, 8, 8);
	layout->setAlignment(Qt::AlignTop);

	m_widget = new QWidget();
	m_widget->setLayout(layout);
	utility::setWidgetBackgroundColor(m_widget, Colori(255, 255, 255, 255));
}

QtCompositeView::~QtCompositeView()
{
}

void QtCompositeView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(m_widget));
}

void QtCompositeView::initView()
{

}

void QtCompositeView::refreshView()
{

}

void QtCompositeView::addViewWidget(View* view)
{
	m_widget->layout()->addWidget(QtViewWidgetWrapper::getWidgetOfView(view));
}
