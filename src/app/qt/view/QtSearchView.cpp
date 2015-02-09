#include "qt/view/QtSearchView.h"

#include "component/controller/SearchController.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "utility/text/TextAccess.h"

QtSearchView::QtSearchView(ViewLayout* viewLayout)
	: SearchView(viewLayout)
	, m_refreshViewFunctor(std::bind(&QtSearchView::doRefreshView, this))
	, m_setTextFunctor(std::bind(&QtSearchView::doSetText, this, std::placeholders::_1))
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

void QtSearchView::setText(const std::string& text)
{
	m_setTextFunctor(text);
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
}

void QtSearchView::doSetText(const std::string& text)
{
	m_widget->setText(text);
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
	std::string css = TextAccess::createFromFile("data/gui/search_view/search_view.css")->getText();

	m_widget->setStyleSheet(css.c_str());

	if (m_widget->getCompleterPopup())
	{
		m_widget->getCompleterPopup()->setStyleSheet(css.c_str());
	}
}
