#include "qt/view/QtScreenSearchView.h"

#include <QToolBar>

#include "component/controller/ScreenSearchController.h"
#include "qt/element/QtScreenSearchBox.h"
#include "qt/utility/utilityQt.h"
#include "qt/view/QtMainView.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "qt/window/QtMainWindow.h"
#include "utility/ResourcePaths.h"

QtScreenSearchView::QtScreenSearchView(ViewLayout* viewLayout)
	: ScreenSearchView(viewLayout)
	, m_controllerProxy(this)
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
			utility::getStyleSheet(ResourcePaths::getGuiPath().concat(
				FilePath("screen_search_view/screen_search_view.css"))).c_str()
		);

		m_widget->refreshStyle();
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

	m_controllerProxy.executeAsTask<ScreenSearchController>(
		[](ScreenSearchController* controller)
		{
			controller->clearMatches();
		}
	);
}
