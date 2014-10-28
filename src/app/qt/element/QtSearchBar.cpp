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
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	m_searchButton = new QPushButton(this);
	m_searchButton->setObjectName("search_button");
	m_searchButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	layout->addWidget(m_searchButton);

	m_searchBox = new QtSmartSearchBox(this);
	m_searchBox->setObjectName("search_box");
	m_searchBox->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	layout->addWidget(m_searchBox);

	m_caseSensitiveButton = new QPushButton(this);
	m_caseSensitiveButton->setObjectName("case_sensitive_button");
	m_caseSensitiveButton->setCheckable(true);
	m_caseSensitiveButton->setToolTip("case sensitive");
	m_caseSensitiveButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	layout->addWidget(m_caseSensitiveButton);

	connect(m_searchButton, SIGNAL(clicked()), m_searchBox, SLOT(search()));
}

QtSearchBar::~QtSearchBar()
{
}

void QtSearchBar::setText(const std::string& text)
{
	m_searchBox->setQuery(text);
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
