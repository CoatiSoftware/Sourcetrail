#include "qt/element/QtSearchBar.h"

#include <QCompleter>
#include <QHBoxLayout>
#include <QPushButton>

#include "qt/element/QtSmartSearchBox.h"

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
	innerLayout->setContentsMargins(7, 3, 5, 3);
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
	m_searchButton->setIcon(QIcon("data/gui/search_view/images/search.png"));
	layout->addWidget(m_searchButton);

	// m_caseSensitiveButton = new QPushButton(this);
	// m_caseSensitiveButton->setObjectName("case_sensitive_button");
	// m_caseSensitiveButton->setCheckable(true);
	// m_caseSensitiveButton->setToolTip("case sensitive");
	// m_caseSensitiveButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	// layout->addWidget(m_caseSensitiveButton);

	connect(m_searchButton, SIGNAL(clicked()), m_searchBox, SLOT(search()));
}

QtSearchBar::~QtSearchBar()
{
}

QSize QtSearchBar::sizeHint() const
{
	return QSize(400, 100);
}

void QtSearchBar::setText(const std::string& text)
{
	m_searchBox->setQuery(text);
}

void QtSearchBar::setMatch(const SearchMatch& match)
{
	m_searchBox->setQuery(match);
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
