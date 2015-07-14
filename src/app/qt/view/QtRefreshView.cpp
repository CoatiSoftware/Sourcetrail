#include "qt/view/QtRefreshView.h"

#include <sstream>

#include "utility/text/TextAccess.h"

#include "component/controller/RefreshController.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "settings/ApplicationSettings.h"

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
	std::stringstream css;
	css << TextAccess::createFromFile("data/gui/refresh_view/refresh_view.css")->getText();
	css << "* { font-size: " << ApplicationSettings::getInstance()->getFontSize() << "pt; }";

	m_widget->setStyleSheet(css.str().c_str());
}
