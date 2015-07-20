#include "qt/view/QtCompositeView.h"

#include <QBoxLayout>

#include "qt/utility/utilityQt.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "settings/ColorScheme.h"

QtCompositeView::QtCompositeView(ViewLayout* viewLayout, CompositeDirection direction)
	: CompositeView(viewLayout, direction)
	, m_refreshFunctor(std::bind(&QtCompositeView::doRefreshView, this))
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

	doRefreshView();
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
	m_refreshFunctor();
}

void QtCompositeView::doRefreshView()
{
	utility::setWidgetBackgroundColor(m_widget, ColorScheme::getInstance()->getColor("search/background"));
}

void QtCompositeView::addViewWidget(View* view)
{
	m_widget->layout()->addWidget(QtViewWidgetWrapper::getWidgetOfView(view));
}
