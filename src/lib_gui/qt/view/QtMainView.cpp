#include "qt/view/QtMainView.h"

#include "utility/logging/logging.h"

#include "qt/window/QtMainWindow.h"

QtMainView::QtMainView()
	: m_hideStartScreenFunctor(std::bind(&QtMainView::doHideStartScreen, this))
	, m_setTitleFunctor(std::bind(&QtMainView::doSetTitle, this, std::placeholders::_1))
	, m_activateWindowFunctor(std::bind(&QtMainView::doActivateWindow, this))
	, m_updateRecentProjectMenuFunctor(std::bind(&QtMainView::doUpdateRecentProjectMenu, this))
	, m_showLicenseScreenFunctor(std::bind(&QtMainView::doShowLicenseScreen, this))
{
	m_window = std::make_shared<QtMainWindow>();
	m_window->show();
	m_window->init();
}

QtMainView::~QtMainView()
{
}

void QtMainView::addView(View* view)
{
	m_views.push_back(view);
	m_window->addView(view);
}

void QtMainView::removeView(View* view)
{
	std::vector<View*>::iterator it = std::find(m_views.begin(), m_views.end(), view);
	if (it == m_views.end())
	{
		return;
	}

	m_window->removeView(view);
	m_views.erase(it);
}

void QtMainView::showView(View* view)
{
	m_window->showView(view);
}

void QtMainView::hideView(View* view)
{
	m_window->hideView(view);
}

void QtMainView::loadLayout()
{
	m_window->loadLayout();
}

void QtMainView::saveLayout()
{
	m_window->saveLayout();
}

QStatusBar* QtMainView::getStatusBar()
{
	return m_window->statusBar();
}

void QtMainView::setStatusBar(QStatusBar* statusbar)
{
	m_window->setStatusBar(statusbar);
}

void QtMainView::hideStartScreen()
{
	m_hideStartScreenFunctor();
}

void QtMainView::setTitle(const std::string& title)
{
	m_setTitleFunctor(title);
}

void QtMainView::activateWindow()
{
	m_activateWindowFunctor();
}

void QtMainView::updateRecentProjectMenu()
{
	m_updateRecentProjectMenuFunctor();
}

void QtMainView::showLicenseScreen()
{
	m_showLicenseScreenFunctor();
}

void QtMainView::doUpdateRecentProjectMenu()
{
	m_window->updateRecentProjectMenu();
}

void QtMainView::doHideStartScreen()
{
	m_window->hideStartScreen();
}

void QtMainView::doSetTitle(const std::string& title)
{
	m_window->setWindowTitle(QString::fromStdString(title));
}

void QtMainView::doActivateWindow()
{
	// It's platform dependent which of these commands does the right thing, for now we just use them all at once.
	m_window->activateWindow();
	m_window->raise();
	m_window->setFocus(Qt::ActiveWindowFocusReason);
}

void QtMainView::doShowLicenseScreen()
{
	m_window->forceEnterLicense();
}
