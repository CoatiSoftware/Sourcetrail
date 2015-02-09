#include "qt/view/QtRefreshView.h"

#include "component/controller/RefreshController.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "utility/text/TextAccess.h"

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
}

void QtRefreshView::setStyleSheet()
{
	std::string css = TextAccess::createFromFile("data/gui/refresh_view/refresh_view.css")->getText();

	m_widget->setStyleSheet(css.c_str());
}
