#include "qt/view/QtSearchView.h"

#include "qt/utility/utilityQt.h"

#include "utility/ResourcePaths.h"

#include "component/controller/SearchController.h"
#include "qt/view/QtViewWidgetWrapper.h"

QtSearchView::QtSearchView(ViewLayout* viewLayout)
	: SearchView(viewLayout)
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
	m_onQtThread([this]()
	{
		setStyleSheet();
		m_widget->refreshStyle();
	});
}

std::string QtSearchView::getQuery() const
{
	return m_widget->query().toStdString();
}

void QtSearchView::setMatches(const std::vector<SearchMatch>& matches)
{
	m_onQtThread([=]()
	{
		m_widget->setMatches(matches);
	});
}

void QtSearchView::setFocus()
{
	m_onQtThread([this]()
	{
		getViewLayout()->showView(this);
		m_widget->setFocus();
	});
}

void QtSearchView::findFulltext()
{
	m_onQtThread([this]()
	{
		getViewLayout()->showView(this);
		m_widget->findFulltext();
	});
}

void QtSearchView::setAutocompletionList(const std::vector<SearchMatch>& autocompletionList)
{
	m_onQtThread([=]()
	{
		m_widget->setAutocompletionList(autocompletionList);
		setStyleSheet();
	});
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
