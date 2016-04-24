#include "qt/view/QtMainView.h"

#include <chrono>
#include <thread>

#include <QMessageBox>

#include "utility/logging/logging.h"

#include "qt/window/QtMainWindow.h"

QtMainView::QtMainView()
	: m_createNewProjectFromSolutionFunctor(std::bind(&QtMainView::doCreateNewProjectFromSolution, this, std::placeholders::_1, std::placeholders::_2))
	, m_showStartScreenFunctor(std::bind(&QtMainView::doShowStartScreen, this))
	, m_hideStartScreenFunctor(std::bind(&QtMainView::doHideStartScreen, this))
	, m_setTitleFunctor(std::bind(&QtMainView::doSetTitle, this, std::placeholders::_1))
	, m_activateWindowFunctor(std::bind(&QtMainView::doActivateWindow, this))
	, m_updateRecentProjectMenuFunctor(std::bind(&QtMainView::doUpdateRecentProjectMenu, this))
	, m_forceLicenseScreenFunctor(std::bind(&QtMainView::doForceLicenseScreen, this, std::placeholders::_1))
	, m_confirmFunctor(std::bind(&QtMainView::doConfirm, this, std::placeholders::_1, std::placeholders::_2))
{
	m_window = std::make_shared<QtMainWindow>();
	m_window->show();
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

int QtMainView::confirm(const std::string& message, const std::vector<std::string>& options)
{
	m_confirmDone = false;

	m_confirmFunctor(message, options);

	while (true)
	{
		{
			std::lock_guard<std::mutex> lock(m_confirmMutex);

			if (m_confirmDone)
			{
				break;
			}
		}

		const int SLEEP_TIME_MS = 25;
		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));
	}

	return m_confirmResult;
}

void QtMainView::handleMessage(MessageForceEnterLicense* message)
{
	m_forceLicenseScreenFunctor(message->licenseExpired);
}

void QtMainView::handleMessage(MessageProjectNew* message)
{
	if (message->ideId.size() != 0 && message->solutionPath.size() != 0)
	{
		m_createNewProjectFromSolutionFunctor(message->ideId, message->solutionPath);
	}
}

void QtMainView::handleMessage(MessageShowStartScreen* message)
{
	m_showStartScreenFunctor();
}

void QtMainView::doCreateNewProjectFromSolution(const std::string& ideId, const std::string& solutionPath)
{
	m_window->newProjectFromSolution(ideId, solutionPath);
}

void QtMainView::doShowStartScreen()
{
	m_window->showStartScreen();
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

void QtMainView::doUpdateRecentProjectMenu()
{
	m_window->updateRecentProjectMenu();
}

void QtMainView::doForceLicenseScreen(bool expired)
{
	m_window->forceEnterLicense(expired);
}

void QtMainView::doConfirm(const std::string& message, const std::vector<std::string>& options)
{
	QMessageBox msgBox;
	msgBox.setText(message.c_str());

	for (const std::string& option : options)
	{
		msgBox.addButton(option.c_str(), QMessageBox::AcceptRole);
	}

	msgBox.exec();

	m_confirmResult = -1;

	for (int i = 0; i < msgBox.buttons().size(); i++)
	{
		if (msgBox.clickedButton() == msgBox.buttons().at(i))
		{
			m_confirmResult = i;
			break;
		}
	}

	std::lock_guard<std::mutex> lock(m_confirmMutex);
	m_confirmDone = true;
}
