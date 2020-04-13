#include "QtSearchView.h"

#include "../../../lib/app/paths/ResourcePaths.h"

#include "../element/search/QtSearchBar.h"
#include "QtViewWidgetWrapper.h"
#include "../utility/utilityQt.h"

QtSearchView::QtSearchView(ViewLayout* viewLayout): SearchView(viewLayout)
{
	m_widget = new QtSearchBar();
}

void QtSearchView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(m_widget));
}

void QtSearchView::refreshView()
{
	m_onQtThread([this]() {
		setStyleSheet();
		m_widget->refreshStyle();
	});
}

std::wstring QtSearchView::getQuery() const
{
	return m_widget->query().toStdWString();
}

void QtSearchView::setMatches(const std::vector<SearchMatch>& matches)
{
	m_onQtThread([=]() { m_widget->setMatches(matches); });
}

void QtSearchView::setFocus()
{
	m_onQtThread([this]() {
		getViewLayout()->showView(this);
		m_widget->setFocus();
	});
}

void QtSearchView::findFulltext()
{
	m_onQtThread([this]() {
		getViewLayout()->showView(this);
		m_widget->findFulltext();
	});
}

void QtSearchView::setAutocompletionList(const std::vector<SearchMatch>& autocompletionList)
{
	m_onQtThread([=]() { m_widget->setAutocompletionList(autocompletionList); });
}

void QtSearchView::setStyleSheet()
{
	const std::string css = utility::getStyleSheet(
		ResourcePaths::getGuiPath().concatenate(L"search_view/search_view.css"));

	m_widget->setStyleSheet(css.c_str());

	if (m_widget->getCompleterPopup())
	{
		m_widget->getCompleterPopup()->setStyleSheet(css.c_str());
	}
}
