#include "qt/view/QtMainView.h"

#include "qt/window/QtMainWindow.h"

QtMainView::QtMainView()
{
	m_window = std::make_shared<QtMainWindow>();
	m_window->show();
}

QtMainView::~QtMainView()
{
}

QtMainWindow* QtMainView::getMainWindow() const
{
	return m_window.get();
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
	m_onQtThread(
		[=]()
		{
			m_window->showView(view);
		}
	);
}

void QtMainView::hideView(View* view)
{
	m_onQtThread(
		[=]()
		{
			m_window->hideView(view);
		}
	);
}

View* QtMainView::findFloatingView(const std::string& name) const
{
	return m_window->findFloatingView(name);
}

void QtMainView::loadLayout()
{
	m_window->loadLayout();
}

void QtMainView::saveLayout()
{
	m_window->saveLayout();
}

void QtMainView::refreshView()
{
	m_onQtThread(
		[=]()
		{
			m_window->refreshStyle();
		}
	);
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
	m_onQtThread(
		[=]()
		{
			m_window->hideStartScreen();
		}
	);
}

void QtMainView::setTitle(const std::wstring& title)
{
	m_onQtThread(
		[=]()
		{
			m_window->setWindowTitle(QString::fromStdWString(title));
		}
	);
}

void QtMainView::activateWindow()
{
	m_onQtThread(
		[=]()
		{
			// It's platform dependent which of these commands does the right thing, for now we just use them all at once.
			m_window->setEnabled(true);
			m_window->raise();
			m_window->setFocus(Qt::ActiveWindowFocusReason);
		}
	);
}

void QtMainView::updateRecentProjectMenu()
{
	m_onQtThread(
		[=]()
		{
			m_window->updateRecentProjectMenu();
		}
	);
}

void QtMainView::updateHistoryMenu(const std::vector<SearchMatch>& history)
{
	m_onQtThread(
		[=]()
		{
			m_window->updateHistoryMenu(history);
		}
	);
}

void QtMainView::updateBookmarksMenu(const std::vector<std::shared_ptr<Bookmark>>& bookmarks)
{
	m_onQtThread(
		[=]()
		{
			m_window->updateBookmarksMenu(bookmarks);
		}
	);
}

void QtMainView::handleMessage(MessageForceEnterLicense* message)
{
	LicenseChecker::LicenseState state = message->state;

	m_onQtThread(
		[=]()
		{
			m_window->forceEnterLicense(state);
		}
	);
}

void QtMainView::handleMessage(MessageLoadProject* message)
{
	bool showStartWindow = message->projectSettingsFilePath.empty();

	m_onQtThread(
		[=]()
		{
			m_window->loadWindow(showStartWindow);
		}
	);
}

void QtMainView::handleMessage(MessageProjectEdit* message)
{
	m_onQtThread(
		[=]()
		{
			m_window->editProject();
		}
	);
}

void QtMainView::handleMessage(MessageProjectNew* message)
{
	FilePath cdbPath = message->cdbPath;

	m_onQtThread(
		[=]()
		{
			m_window->newProjectFromCDB(cdbPath);
		}
	);
}
