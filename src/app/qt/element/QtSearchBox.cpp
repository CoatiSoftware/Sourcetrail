#include "qt/element/QtSearchBox.h"

#include <QCompleter>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>

#include "data/query/QueryTree.h"
#include "utility/messaging/type/MessageSearch.h"
#include "utility/messaging/type/MessageSearchAutocomplete.h"
#include "utility/utilityString.h"

QtSearchBox::QtSearchBox()
	: m_preventQueryChange(false)
{
	setObjectName("search_view");

	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	m_searchButton = new QPushButton(this);
	m_searchButton->setObjectName("search_button");
	m_searchButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	connect(m_searchButton, SIGNAL(clicked()), this, SLOT(onSearchButtonClick()));
	layout->addWidget(m_searchButton);

	m_searchBox = new QLineEdit(this);
	m_searchBox->setObjectName("search_box");
	m_searchBox->setPlaceholderText("Please enter your search string.");
	m_searchBox->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	connect(m_searchBox, SIGNAL(returnPressed()), this, SLOT(onSearchButtonClick()));
	connect(m_searchBox, SIGNAL(textEdited(const QString&)), this, SLOT(onSearchQueryEdited(const QString&)));
	connect(m_searchBox, SIGNAL(textChanged(const QString&)), this, SLOT(onSearchQueryChanged(const QString&)));
	layout->addWidget(m_searchBox);

	m_caseSensitiveButton = new QPushButton(this);
	m_caseSensitiveButton->setObjectName("case_sensitive_button");
	m_caseSensitiveButton->setCheckable(true);
	m_caseSensitiveButton->setToolTip("case sensitive");
	m_caseSensitiveButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	layout->addWidget(m_caseSensitiveButton);
}

QtSearchBox::~QtSearchBox()
{
}

void QtSearchBox::setText(const std::string& text)
{
	if (m_searchBox->text() != text.c_str())
	{
		m_searchBox->setText(text.c_str());
	}
}

void QtSearchBox::setFocus()
{
	m_searchBox->setFocus(Qt::ShortcutFocusReason);
}

void QtSearchBox::setAutocompletionList(const std::vector<SearchIndex::SearchMatch>& autocompletionList)
{
	m_matches = autocompletionList;

	QStringList wordList;
	for (const SearchIndex::SearchMatch& match: autocompletionList)
	{
		wordList << match.fullName.c_str();
	}

	QCompleter *completer = new QCompleter(wordList, m_searchBox);
	completer->popup()->setObjectName("search_box_popup");
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	m_searchBox->setCompleter(completer);
	completer->complete();

	connect(completer, SIGNAL(highlighted(const QModelIndex&)), this, SLOT(onSearchCompletionHighlighted(const QModelIndex&)));
	connect(completer, SIGNAL(activated(const QString&)), this, SLOT(onSearchCompletionActivated(const QString&)));
}

QAbstractItemView* QtSearchBox::getCompleterPopup()
{
	if (m_searchBox->completer())
	{
		return m_searchBox->completer()->popup();
	}
	return nullptr;
}

void QtSearchBox::onSearchButtonClick()
{
	m_query = m_searchBox->text().toStdString();
	MessageSearch(m_query).dispatch();
}

void QtSearchBox::onSearchQueryEdited(const QString& text)
{
	m_query = text.toStdString();
	m_oldQuery = m_query;

	std::deque<std::string> tokens = QueryTree::tokenizeQuery(text.toStdString());
	if (tokens.size())
	{
		MessageSearchAutocomplete(tokens.back()).dispatch();
	}
}

void QtSearchBox::onSearchQueryChanged(const QString& text)
{
	if (m_preventQueryChange)
	{
		m_preventQueryChange = false;
		setText(m_query);
	}
}

void QtSearchBox::onSearchCompletionHighlighted(const QModelIndex& index)
{
	if (index.row() < 0 || index.row() >= int(m_matches.size()))
	{
		m_query = m_oldQuery;
	}
	else
	{
		std::deque<std::string> tokens = QueryTree::tokenizeQuery(m_query);
		if (tokens.size())
		{
			tokens.pop_back();
		}

		std::string match = m_matches[index.row()].encodeForQuery();
		tokens.push_back(match);

		m_query = utility::join(tokens, "");
	}

	setText(m_query);
	m_preventQueryChange = true;
}

void QtSearchBox::onSearchCompletionActivated(const QString& text)
{
	setText(m_query);
	m_preventQueryChange = true;
}
