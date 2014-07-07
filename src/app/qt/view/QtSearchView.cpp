#include "qt/view/QtSearchView.h"

#include <QtWidgets>

#include "qt/element/QtButton.h"
#include "qt/element/QtEditBox.h"
#include "qt/QtWidgetWrapper.h"
#include "qt/utility/utilityQt.h"
#include "component/controller/SearchController.h"

QtSearchView::QtSearchView(ViewLayout* viewLayout)
	: SearchView(viewLayout)
	, m_setTextFunctor(std::bind(&QtSearchView::doSetText, this, std::placeholders::_1))
	, m_setAutocompletionListFunctor(std::bind(&QtSearchView::doSetAutocompletionList, this, std::placeholders::_1))
{
}

QtSearchView::~QtSearchView()
{
}

void QtSearchView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtWidgetWrapper>(std::make_shared<QFrame>()));
}

void QtSearchView::initGui()
{
	QWidget* widget = QtWidgetWrapper::getWidgetOfView(this);
	utility::setWidgetBackgroundColor(widget, Colori(190, 190, 190, 255));

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);
	layout->setSpacing(3);
	layout->setContentsMargins(3, 3, 3, 3);
	widget->setLayout(layout);

	m_searchBox = new QtEditBox(widget);
	m_searchBox->setPlaceholderText("Please enter your search string.");
	m_searchBox->setCallbackOnReturnPressed(std::bind(&QtSearchView::onSearchButtonClick, this));

	std::vector<std::string> autocompletionList;
	autocompletionList.push_back("tralala");
	autocompletionList.push_back("lalila");
	setAutocompletionList(autocompletionList);

	m_searchButton = new QtButton(widget);
	m_searchButton->setText("Search");
	m_searchButton->setCallbackOnClick(std::bind(&QtSearchView::onSearchButtonClick, this));

	widget->layout()->addWidget(m_searchBox);
	widget->layout()->addWidget(m_searchButton);
}

void QtSearchView::setText(const std::string& s)
{
	m_setTextFunctor(s);
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

void QtSearchView::doSetText(const std::string& s)
{
	if (m_searchBox->text() != s.c_str())
	{
		m_searchBox->setText(s.c_str());
	}
}

void QtSearchView::doSetAutocompletionList(const std::vector<std::string>& autocompletionList)
{
	QStringList wordList;
	for (const std::string& s: autocompletionList)
		wordList << s.c_str();
	QCompleter *completer = new QCompleter(wordList, m_searchBox);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	m_searchBox->setCompleter(completer);
}
