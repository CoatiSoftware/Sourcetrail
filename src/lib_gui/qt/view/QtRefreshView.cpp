#include "qt/view/QtRefreshView.h"

#include "qt/utility/utilityQt.h"

#include "utility/ResourcePaths.h"

#include "component/controller/RefreshController.h"
#include "qt/view/QtViewWidgetWrapper.h"

QtRefreshView::QtRefreshView(ViewLayout* viewLayout)
	: RefreshView(viewLayout)
	, m_refreshViewFunctor(std::bind(&QtRefreshView::doRefreshView, this))
{
	m_widget = new QtRefreshBar();
	setStyleSheet();
}

QtRefreshView::~QtRefreshView()
{
}

void QtRefreshView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(m_widget));
}

void QtRefreshView::initView()
{
}

void QtRefreshView::refreshView()
{
	m_refreshViewFunctor();
}

void QtRefreshView::doRefreshView()
{
	setStyleSheet();
	m_widget->refreshStyle();
}

void QtRefreshView::setStyleSheet()
{
	m_widget->setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath() + "refresh_view/refresh_view.css").c_str());
}
