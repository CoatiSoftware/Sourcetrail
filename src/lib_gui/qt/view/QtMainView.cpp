#include "QtMainView.h"

#include "MessageRefreshUIState.h"
#include "QtMainWindow.h"
#include "QtViewWidgetWrapper.h"
#include "utilityApp.h"

QtMainView::QtMainView(const ViewFactory* viewFactory, StorageAccess* storageAccess)
	: MainView(viewFactory, storageAccess)
{
	m_window = new QtMainWindow();
	m_window->show();
}

QtMainView::~QtMainView()
{
	// clear components to avoid double deletion of views when destroying m_window
	m_componentManager.clear();
	m_window->deleteLater();
}

QtMainWindow* QtMainView::getMainWindow() const
{
	return m_window;
}

void QtMainView::addView(View* view)
{
	m_views.push_back(view);
	m_window->addView(view);
}

void QtMainView::overrideView(View* view)
{
	m_window->overrideView(view);
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
	m_onQtThread([=]() { m_window->showView(view); });
}

void QtMainView::hideView(View* view)
{
	m_onQtThread([=]() { m_window->hideView(view); });
}

void QtMainView::setViewEnabled(View* view, bool enabled)
{
	m_onQtThread([=]() {
		QWidget* widget = QtViewWidgetWrapper::getWidgetOfView(view);
		widget->setEnabled(enabled);
	});
}

View* QtMainView::findFloatingView(const std::string& name) const
{
	return m_window->findFloatingView(name);
}

void QtMainView::showOriginalViews()
{
	for (View* view: m_views)
	{
		m_window->overrideView(view);
	}
}

void QtMainView::loadLayout()
{
	m_window->loadLayout();
}

void QtMainView::saveLayout()
{
	m_window->saveLayout();
}

void QtMainView::loadWindow(bool showStartWindow)
{
	m_onQtThread([=]() { m_window->loadWindow(showStartWindow); });
}

void QtMainView::refreshView()
{
	m_onQtThread([=]() { m_window->refreshStyle(); });
}

void QtMainView::refreshUIState(bool isAfterIndexing)
{
	m_onQtThread([=]() { MessageRefreshUIState(isAfterIndexing).dispatch(); });
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
	m_onQtThread([=]() { m_window->hideStartScreen(); });
}

void QtMainView::setTitle(const std::wstring& title)
{
	m_onQtThread([=]() { m_window->setWindowTitle(QString::fromStdWString(title)); });
}

void QtMainView::activateWindow()
{
	m_onQtThread([=]() {
		// It's platform dependent which of these commands does the right thing, for now we just use
		// them all at once.
		m_window->activateWindow();
		m_window->setEnabled(true);
		m_window->raise();
		m_window->setFocus(Qt::ActiveWindowFocusReason);
		m_window->setWindowState(m_window->windowState() & ~Qt::WindowMinimized);
	});
}

void QtMainView::updateRecentProjectMenu()
{
	m_onQtThread([=]() { m_window->updateRecentProjectsMenu(); });
}

void QtMainView::updateHistoryMenu(std::shared_ptr<MessageBase> message)
{
	m_onQtThread([=]() { m_window->updateHistoryMenu(message); });
}

void QtMainView::clearHistoryMenu()
{
	m_onQtThread([=]() { m_window->clearHistoryMenu(); });
}

void QtMainView::updateBookmarksMenu(const std::vector<std::shared_ptr<Bookmark>>& bookmarks)
{
	m_onQtThread([=]() { m_window->updateBookmarksMenu(bookmarks); });
}

void QtMainView::clearBookmarksMenu()
{
	updateBookmarksMenu({});
}

void QtMainView::handleMessage(MessageProjectEdit* message)
{
	m_onQtThread([=]() { m_window->editProject(); });
}

void QtMainView::handleMessage(MessageProjectNew* message)
{
	FilePath cdbPath = message->cdbPath;

	m_onQtThread([=]() { m_window->newProjectFromCDB(cdbPath); });
}

void QtMainView::handleMessage(MessageWindowChanged* message)
{
	// Fixes an issue where newly added QtWidgets don't fully respond to focus events on macOS
	if (utility::getOsType() == OS_MAC)
	{
		m_onQtThread([=]() {
			m_window->hide();
			m_window->show();
		});
	}
}
