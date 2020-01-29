#include "QtSearchBar.h"

#include <QHBoxLayout>

#include "MessageActivateFullTextSearch.h"
#include "MessageActivateOverview.h"
#include "MessageSearch.h"
#include "MessageSearchAutocomplete.h"
#include "QtSearchBarButton.h"
#include "QtSmartSearchBox.h"
#include "ResourcePaths.h"

QtSearchBar::QtSearchBar()
{
	setObjectName(QStringLiteral("search_bar"));

	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	m_homeButton = new QtSearchBarButton(
		ResourcePaths::getGuiPath().concatenate(L"search_view/images/home.png"));
	m_homeButton->setObjectName(QStringLiteral("home_button"));
	m_homeButton->setToolTip(QStringLiteral("to overview"));
	layout->addWidget(m_homeButton);
	connect(m_homeButton, &QPushButton::clicked, this, &QtSearchBar::homeButtonClicked);

	m_searchBoxContainer = new QWidget(this);
	m_searchBoxContainer->setObjectName(QStringLiteral("search_box_container"));
	m_searchBoxContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	layout->addWidget(m_searchBoxContainer);

	QBoxLayout* innerLayout = new QHBoxLayout();
	innerLayout->setContentsMargins(12, 3, 5, 2);
	m_searchBoxContainer->setLayout(innerLayout);

	m_searchBox = new QtSmartSearchBox(QStringLiteral("Search"), true, m_searchBoxContainer);
	m_searchBox->setMinimumWidth(100);
	m_searchBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	innerLayout->addWidget(m_searchBox);

	connect(m_searchBox, &QtSmartSearchBox::autocomplete, this, &QtSearchBar::requestAutocomplete);
	connect(m_searchBox, &QtSmartSearchBox::search, this, &QtSearchBar::requestSearch);
	connect(
		m_searchBox, &QtSmartSearchBox::fullTextSearch, this, &QtSearchBar::requestFullTextSearch);

	m_searchButton = new QtSearchBarButton(
		ResourcePaths::getGuiPath().concatenate(L"search_view/images/search.png"));
	m_searchButton->setObjectName(QStringLiteral("search_button"));
	m_searchButton->setToolTip(QStringLiteral("search"));
	layout->addWidget(m_searchButton);

	connect(m_searchButton, &QPushButton::clicked, m_searchBox, &QtSmartSearchBox::startSearch);

	refreshStyle();
}

QtSearchBar::~QtSearchBar() {}

QSize QtSearchBar::sizeHint() const
{
	return QSize(400, 100);
}

QString QtSearchBar::query() const
{
	return m_searchBox->text();
}

void QtSearchBar::setMatches(const std::vector<SearchMatch>& matches)
{
	m_searchBox->setMatches(matches);
}

void QtSearchBar::setFocus()
{
	m_searchBox->setFocus();
}

void QtSearchBar::findFulltext()
{
	m_searchBox->findFulltext();
}

void QtSearchBar::setAutocompletionList(const std::vector<SearchMatch>& autocompletionList)
{
	m_searchBox->setAutocompletionList(autocompletionList);
}

QAbstractItemView* QtSearchBar::getCompleterPopup()
{
	return m_searchBox->getCompleter()->popup();
}

void QtSearchBar::refreshStyle()
{
	m_searchBox->refreshStyle();
}

void QtSearchBar::homeButtonClicked()
{
	MessageActivateOverview().dispatch();
}

void QtSearchBar::requestAutocomplete(const std::wstring& query, NodeTypeSet acceptedNodeTypes)
{
	MessageSearchAutocomplete(query, acceptedNodeTypes).dispatch();
}

void QtSearchBar::requestSearch(const std::vector<SearchMatch>& matches, NodeTypeSet acceptedNodeTypes)
{
	MessageSearch(matches, acceptedNodeTypes).dispatch();
}

void QtSearchBar::requestFullTextSearch(const std::wstring& query, bool caseSensitive)
{
	MessageActivateFullTextSearch(query, caseSensitive).dispatch();
}
