#include "qt/element/QtSearchBar.h"

#include <QCompleter>
#include <QHBoxLayout>
#include <QPushButton>

#include "qt/element/QtSmartSearchBox.h"
#include "qt/utility/utilityQt.h"
#include "settings/ApplicationSettings.h"
#include "settings/ColorScheme.h"

QtSearchBar::QtSearchBar()
{
	setObjectName("search_bar");

	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

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

	connect(m_searchButton, SIGNAL(clicked()), m_searchBox, SLOT(search()));

	refreshStyle();
}

QtSearchBar::~QtSearchBar()
{
}

QSize QtSearchBar::sizeHint() const
{
	return QSize(400, 100);
}

void QtSearchBar::setMatches(const std::deque<SearchMatch>& matches)
{
	m_searchBox->setMatches(matches);
}

void QtSearchBar::setFocus()
{
	m_searchBox->setFocus();
}

void QtSearchBar::setAutocompletionList(const std::vector<SearchMatch>& autocompletionList)
{
	m_searchBox->setAutocompletionList(autocompletionList);
}

QAbstractItemView* QtSearchBar::getCompleterPopup()
{
	if (m_searchBox->completer())
	{
		return m_searchBox->completer()->popup();
	}
	return nullptr;
}

void QtSearchBar::refreshStyle()
{
	m_searchBox->setFixedHeight(std::max(ApplicationSettings::getInstance()->getFontSize() + 11, 25));
	m_searchButton->setFixedHeight(m_searchBox->height() + 5);

	m_searchButton->setIcon(utility::colorizePixmap(
		QPixmap("data/gui/search_view/images/search.png"),
		ColorScheme::getInstance()->getColor("search/button/icon").c_str()
	));
}
