#include "qt/view/QtSearchView.h"

#include <QAbstractItemView>
#include <QCompleter>
#include <QFrame>
#include <QHBoxLayout>

#include "component/controller/SearchController.h"
#include "qt/element/QtButton.h"
#include "qt/element/QtEditBox.h"
#include "qt/utility/utilityQt.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "utility/text/TextAccess.h"

QtSearchView::QtSearchView(ViewLayout* viewLayout)
	: SearchView(viewLayout)
	, m_refreshViewFunctor(std::bind(&QtSearchView::doRefreshView, this))
	, m_setTextFunctor(std::bind(&QtSearchView::doSetText, this, std::placeholders::_1))
	, m_setFocusFunctor(std::bind(&QtSearchView::doSetFocus, this))
	, m_setAutocompletionListFunctor(std::bind(&QtSearchView::doSetAutocompletionList, this, std::placeholders::_1))
{
}

QtSearchView::~QtSearchView()
{
}

void QtSearchView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(std::make_shared<QFrame>()));
}

void QtSearchView::initView()
{
	QWidget* widget = QtViewWidgetWrapper::getWidgetOfView(this);
	widget->setObjectName("search_view");

	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setAlignment(Qt::AlignTop);
	widget->setLayout(layout);

	m_searchButton = new QtButton(widget);
	m_searchButton->setObjectName("search_button");
	m_searchButton->setCallbackOnClick(std::bind(&QtSearchView::onSearchButtonClick, this));
	widget->layout()->addWidget(m_searchButton);

	m_searchBox = new QtEditBox(widget);
	m_searchBox->setObjectName("search_box");
	m_searchBox->setPlaceholderText("Please enter your search string.");
	m_searchBox->setCallbackOnReturnPressed(std::bind(&QtSearchView::onSearchButtonClick, this));
	widget->layout()->addWidget(m_searchBox);

	m_caseSensitiveButton = new QtButton(widget);
	m_caseSensitiveButton->setObjectName("case_sensitive_button");
	m_caseSensitiveButton->setCheckable(true);
	m_caseSensitiveButton->setToolTip("case sensitive");
	widget->layout()->addWidget(m_caseSensitiveButton);

	setStyleSheet();
}

void QtSearchView::refreshView()
{
	m_refreshViewFunctor();
}

void QtSearchView::setText(const std::string& s)
{
	m_setTextFunctor(s);
}

void QtSearchView::setFocus()
{
	m_setFocusFunctor();
}

void QtSearchView::setAutocompletionList(const std::vector<std::string>& autocompletionList)
{
	m_setAutocompletionListFunctor(autocompletionList);
}

void QtSearchView::onSearchButtonClick()
{
	SearchController* controller = getController();
	if (controller)
	{
		controller->search(m_searchBox->text().toStdString());
	}
}

void QtSearchView::doRefreshView()
{
	setStyleSheet();
}

void QtSearchView::doSetText(const std::string& s)
{
	if (m_searchBox->text() != s.c_str())
	{
		m_searchBox->setText(s.c_str());
	}
}

void QtSearchView::doSetFocus()
{
	getViewLayout()->showView(this);
	m_searchBox->setFocus(Qt::ShortcutFocusReason);
}

void QtSearchView::doSetAutocompletionList(const std::vector<std::string>& autocompletionList)
{
	QStringList wordList;
	for (const std::string& s: autocompletionList)
	{
		wordList << s.c_str();
	}

	QCompleter *completer = new QCompleter(wordList, m_searchBox);
	completer->popup()->setObjectName("search_box_popup");
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	m_searchBox->setCompleter(completer);
	setStyleSheet();
}

void QtSearchView::setStyleSheet()
{
	std::string css = TextAccess::createFromFile("data/gui/search_view/search_view.css")->getText();

	QWidget* widget = QtViewWidgetWrapper::getWidgetOfView(this);
	widget->setStyleSheet(css.c_str());

	if (m_searchBox->completer())
	{
		m_searchBox->completer()->popup()->setStyleSheet(css.c_str());
	}
}
