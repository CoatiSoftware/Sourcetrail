#include "QtSearchBar.h"

#include <QHBoxLayout>

#include "MessageActivateAll.h"
#include "ResourcePaths.h"

#include "QtSearchBarButton.h"
#include "QtSmartSearchBox.h"

QtSearchBar::QtSearchBar()
{
	setObjectName("search_bar");

	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	m_homeButton = new QtSearchBarButton(ResourcePaths::getGuiPath().concatenate(L"search_view/images/home.png"));
	m_homeButton->setObjectName("home_button");
	m_homeButton->setToolTip("to overview");
	layout->addWidget(m_homeButton);
	connect(m_homeButton, &QPushButton::clicked, this, &QtSearchBar::homeButtonClicked);

	m_searchBoxContainer = new QWidget(this);
	m_searchBoxContainer->setObjectName("search_box_container");
	m_searchBoxContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	layout->addWidget(m_searchBoxContainer);

	QBoxLayout* innerLayout = new QHBoxLayout();
	innerLayout->setContentsMargins(12, 3, 5, 2);
	m_searchBoxContainer->setLayout(innerLayout);

	m_searchBox = new QtSmartSearchBox(m_searchBoxContainer);
	m_searchBox->setObjectName("search_box");
	m_searchBox->setAttribute(Qt::WA_MacShowFocusRect, 0); // remove blue focus box on Mac
	m_searchBox->setMinimumWidth(100);
	m_searchBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	innerLayout->addWidget(m_searchBox);

	m_searchButton = new QtSearchBarButton(ResourcePaths::getGuiPath().concatenate(L"search_view/images/search.png"));
	m_searchButton->setObjectName("search_button");
	m_searchButton->setToolTip("search");
	layout->addWidget(m_searchButton);

	connect(m_searchButton, &QPushButton::clicked, m_searchBox, &QtSmartSearchBox::search);

	refreshStyle();
}

QtSearchBar::~QtSearchBar()
{
}

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
	MessageActivateAll().dispatch();
}
