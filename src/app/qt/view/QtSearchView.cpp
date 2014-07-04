#include "qt/view/QtSearchView.h"

#include <QtWidgets>

#include "qt/element/QtButton.h"
#include "qt/element/QtEditBox.h"
#include "qt/QtWidgetWrapper.h"
#include "qt/utility/utilityQt.h"
#include "component/controller/SearchController.h"

QtSearchView::QtSearchView(ViewLayout* viewLayout)
	: SearchView(viewLayout)
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
	m_searchBox->setCallbackOnReturnPressed(std::bind(&QtSearchView::onSeachButtonClick, this));

	m_searchButton = new QtButton(widget);
	m_searchButton->setText("Search");
	m_searchButton->setCallbackOnClick(std::bind(&QtSearchView::onSeachButtonClick, this));

	widget->layout()->addWidget(m_searchBox);
	widget->layout()->addWidget(m_searchButton);
}

void QtSearchView::setText(const std::string& s) const
{
	if (m_searchBox->text() != s.c_str())
	{
		m_searchBox->setText(s.c_str());
	}
}

void QtSearchView::onSeachButtonClick()
{
	getController()->search(m_searchBox->text().toStdString());
}
