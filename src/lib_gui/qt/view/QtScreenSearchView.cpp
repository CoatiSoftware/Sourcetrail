#include "QtScreenSearchView.h"

#include <QToolBar>

#include "QtScreenSearchBox.h"
#include "QtMainView.h"
#include "QtViewWidgetWrapper.h"
#include "QtMainWindow.h"
#include "ResourcePaths.h"
#include "TabId.h"
#include "utilityQt.h"

QtScreenSearchView::QtScreenSearchView(ViewLayout* viewLayout)
	: ScreenSearchView(viewLayout)
	, m_controllerProxy(this, TabId::app())
{
	m_widget = new QtScreenSearchBox(&m_controllerProxy);

	m_bar = new QToolBar();
	m_bar->setObjectName("screen_search_bar");
	m_bar->setMovable(false);
	m_bar->addWidget(m_widget);

	QtMainWindow* mainWindow = dynamic_cast<QtMainView*>(getViewLayout())->getMainWindow();

	QObject::connect(m_widget, &QtScreenSearchBox::closePressed, this, &QtScreenSearchView::hide);
	QObject::connect(mainWindow, &QtMainWindow::showScreenSearch, this, &QtScreenSearchView::show);
	QObject::connect(mainWindow, &QtMainWindow::hideScreenSearch, this, &QtScreenSearchView::hide);
}

QtScreenSearchView::~QtScreenSearchView()
{
}

void QtScreenSearchView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(m_widget));
}

void QtScreenSearchView::initView()
{
}

void QtScreenSearchView::refreshView()
{
	m_onQtThread([=]()
	{
		m_bar->setStyleSheet(
			utility::getStyleSheet(
				ResourcePaths::getGuiPath().concatenate(
					L"screen_search_view/screen_search_view.css"
				)
			).c_str()
		);
	});
}

void QtScreenSearchView::setMatchCount(size_t matchCount)
{
	m_onQtThread([=]()
	{
		m_widget->setMatchCount(matchCount);
	});
}

void QtScreenSearchView::setMatchIndex(size_t matchIndex)
{
	m_onQtThread([=]()
	{
		m_widget->setMatchIndex(matchIndex);
	});
}

void QtScreenSearchView::addResponder(const std::string& name)
{
	m_widget->addResponder(name);
}

void QtScreenSearchView::show()
{
	QtMainWindow* mainWindow = dynamic_cast<QtMainView*>(getViewLayout())->getMainWindow();
	mainWindow->addToolBar(Qt::BottomToolBarArea, m_bar);

	m_bar->show();
	m_widget->setFocus();
}

void QtScreenSearchView::hide()
{
	m_bar->hide();
	m_widget->setMatchCount(0);

	m_controllerProxy.executeAsTask(&ScreenSearchController::clearMatches);
}
