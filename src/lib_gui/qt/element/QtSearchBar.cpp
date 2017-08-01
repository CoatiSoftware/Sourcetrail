#include "qt/element/QtSearchBar.h"

#include <QHBoxLayout>
#include <QPushButton>

#include "utility/messaging/type/MessageSearch.h"
#include "utility/ResourcePaths.h"

#include "qt/element/QtSmartSearchBox.h"
#include "qt/utility/utilityQt.h"
#include "settings/ApplicationSettings.h"

QtSearchBar::QtSearchBar()
{
	setObjectName("search_bar");

	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	m_homeButton = new QPushButton(this);
	m_homeButton->setObjectName("home_button");
	m_homeButton->setToolTip("to overview");
	m_homeButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	layout->addWidget(m_homeButton);
	connect(m_homeButton, &QPushButton::clicked, this, &QtSearchBar::homeButtonClicked);

	m_searchBoxContainer = new QWidget(this);
	m_searchBoxContainer->setObjectName("search_box_container");
	m_searchBoxContainer->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_searchBoxContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	layout->addWidget(m_searchBoxContainer);

	QBoxLayout* innerLayout = new QHBoxLayout();
	innerLayout->setContentsMargins(7, 3, 5, 2);
	m_searchBoxContainer->setLayout(innerLayout);

	m_searchBox = new QtSmartSearchBox(m_searchBoxContainer);
	m_searchBox->setObjectName("search_box");
	m_searchBox->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_searchBox->setAttribute(Qt::WA_MacShowFocusRect, 0); // remove blue focus box on Mac
	m_searchBox->setMinimumWidth(100);
	m_searchBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	innerLayout->addWidget(m_searchBox);

	m_searchButton = new QPushButton(this);
	m_searchButton->setObjectName("search_button");
	m_searchButton->setToolTip("search");
	m_searchButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
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
	m_searchBox->setFixedHeight(std::max(ApplicationSettings::getInstance()->getFontSize() + 11, 25));
	m_searchButton->setFixedHeight(m_searchBox->height() + 5);
	m_homeButton->setFixedHeight(m_searchBox->height() + 5);

	m_searchButton->setIcon(utility::createButtonIcon(
		ResourcePaths::getGuiPath().str() + "search_view/images/search.png",
		"search/button"
	));

	m_homeButton->setIcon(utility::createButtonIcon(
		ResourcePaths::getGuiPath().str() + "search_view/images/home.png",
		"search/button"
	));

	m_searchBox->refreshStyle();
}

void QtSearchBar::homeButtonClicked()
{
	MessageSearch(std::vector<SearchMatch>(1, SearchMatch::createCommand(SearchMatch::COMMAND_ALL))).dispatch();
}
