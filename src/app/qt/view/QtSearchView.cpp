#include "qt/view/QtSearchView.h"

#include <sstream>

#include "utility/text/TextAccess.h"

#include "component/controller/SearchController.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "settings/ApplicationSettings.h"

QtSearchView::QtSearchView(ViewLayout* viewLayout)
	: SearchView(viewLayout)
	, m_refreshViewFunctor(std::bind(&QtSearchView::doRefreshView, this))
	, m_setMatchesFunctor(std::bind(&QtSearchView::doSetMatches, this, std::placeholders::_1))
	, m_setFocusFunctor(std::bind(&QtSearchView::doSetFocus, this))
	, m_setAutocompletionListFunctor(std::bind(&QtSearchView::doSetAutocompletionList, this, std::placeholders::_1))
{
	m_widget = new QtSearchBar();
	setStyleSheet();
}

QtSearchView::~QtSearchView()
{
}

void QtSearchView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(m_widget));
}

void QtSearchView::initView()
{
}

void QtSearchView::refreshView()
{
	m_refreshViewFunctor();
}

void QtSearchView::setMatches(const std::deque<SearchMatch>& matches)
{
	m_setMatchesFunctor(matches);
}

void QtSearchView::setFocus()
{
	m_setFocusFunctor();
}

void QtSearchView::setAutocompletionList(const std::vector<SearchMatch>& autocompletionList)
{
	m_setAutocompletionListFunctor(autocompletionList);
}

void QtSearchView::doRefreshView()
{
	setStyleSheet();
	m_widget->refreshStyle();
	m_widget->setMatches(std::deque<SearchMatch>());
}

void QtSearchView::doSetMatches(const std::deque<SearchMatch>& matches)
{
	m_widget->setMatches(matches);
}

void QtSearchView::doSetFocus()
{
	getViewLayout()->showView(this);
	m_widget->setFocus();
}

void QtSearchView::doSetAutocompletionList(const std::vector<SearchMatch>& autocompletionList)
{
	m_widget->setAutocompletionList(autocompletionList);
	setStyleSheet();
}

void QtSearchView::setStyleSheet()
{
	std::stringstream css;
	css << TextAccess::createFromFile("data/gui/search_view/search_view.css")->getText();
	css << "* { font-size: " << ApplicationSettings::getInstance()->getFontSize() << "pt; }";
	css << "#search_box, #search_box *, #search_box_popup { font-family: " << ApplicationSettings::getInstance()->getFontName() << "; }";
	css << "#search_box, #search_box * { font-size: " << ApplicationSettings::getInstance()->getFontSize() + 2 << "pt; }";

	m_widget->setStyleSheet(css.str().c_str());

	if (m_widget->getCompleterPopup())
	{
		m_widget->getCompleterPopup()->setStyleSheet(css.str().c_str());
	}
}
