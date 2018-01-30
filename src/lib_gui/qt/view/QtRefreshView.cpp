#include "qt/view/QtRefreshView.h"

#include "component/controller/RefreshController.h"
#include "qt/utility/utilityQt.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "utility/ResourcePaths.h"

QtRefreshView::QtRefreshView(ViewLayout* viewLayout)
	: RefreshView(viewLayout)
{
	m_widget = new QtRefreshBar();
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
	m_onQtThread([this]()
	{
		setStyleSheet();
		m_widget->refreshStyle();
	});
}

void QtRefreshView::setStyleSheet()
{
	m_widget->setStyleSheet(utility::getStyleSheet(
		ResourcePaths::getGuiPath().concatenate(L"refresh_view/refresh_view.css")).c_str());
}
