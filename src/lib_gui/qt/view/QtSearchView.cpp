#include "qt/view/QtSearchView.h"

#include "qt/utility/utilityQt.h"

#include "utility/ResourcePaths.h"

#include "component/controller/SearchController.h"
#include "qt/view/QtViewWidgetWrapper.h"

QtSearchView::QtSearchView(ViewLayout* viewLayout)
	: SearchView(viewLayout)
	, m_refreshViewFunctor(std::bind(&QtSearchView::doRefreshView, this))
	, m_setMatchesFunctor(std::bind(&QtSearchView::doSetMatches, this, std::placeholders::_1))
	, m_setFocusFunctor(std::bind(&QtSearchView::doSetFocus, this))
	, m_findFulltextFunctor(std::bind(&QtSearchView::doFindFulltext, this))
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

std::string QtSearchView::getQuery() const
{
	return m_widget->query().toStdString();
}

void QtSearchView::setMatches(const std::vector<SearchMatch>& matches)
{
	m_setMatchesFunctor(matches);
}

void QtSearchView::setFocus()
{
	m_setFocusFunctor();
}

void QtSearchView::findFulltext()
{
	m_findFulltextFunctor();
}

void QtSearchView::setAutocompletionList(const std::vector<SearchMatch>& autocompletionList)
{
	m_setAutocompletionListFunctor(autocompletionList);
}

void QtSearchView::doRefreshView()
{
	setStyleSheet();
	m_widget->refreshStyle();
}

void QtSearchView::doSetMatches(const std::vector<SearchMatch>& matches)
{
	m_widget->setMatches(matches);
}

void QtSearchView::doSetFocus()
{
	getViewLayout()->showView(this);
	m_widget->setFocus();
}

void QtSearchView::doFindFulltext()
{
	getViewLayout()->showView(this);
	m_widget->findFulltext();
}

void QtSearchView::doSetAutocompletionList(const std::vector<SearchMatch>& autocompletionList)
{
	m_widget->setAutocompletionList(autocompletionList);
	setStyleSheet();
}

void QtSearchView::setStyleSheet()
{
	std::string css = utility::getStyleSheet(ResourcePaths::getGuiPath().concat(FilePath("search_view/search_view.css")));

	m_widget->setStyleSheet(css.c_str());

	if (m_widget->getCompleterPopup())
	{
		m_widget->getCompleterPopup()->setStyleSheet(css.c_str());
	}
}
