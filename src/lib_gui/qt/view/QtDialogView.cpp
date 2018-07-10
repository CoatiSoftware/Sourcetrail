#include "qt/view/QtDialogView.h"

#include <chrono>
#include <sstream>
#include <thread>

#include <QMessageBox>
#include <QTimer>

#include "data/access/StorageAccess.h"
#include "qt/window/QtIndexingDialog.h"
#include "qt/window/QtMainWindow.h"
#include "qt/window/QtWindow.h"
#include "utility/messaging/type/indexing/MessageIndexingStatus.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/scheduling/TaskLambda.h"
#include "utility/utility.h"
#include "project/Project.h"

QtDialogView::QtDialogView(QtMainWindow* mainWindow, UseCase useCase, StorageAccess* storageAccess)
	: DialogView(useCase, storageAccess)
	, m_mainWindow(mainWindow)
	, m_parentWindow(nullptr)
	, m_windowStack(this)
	, m_resultReady(false)
{
}

QtDialogView::~QtDialogView()
{
	m_resultReady = true;
}

bool QtDialogView::dialogsHidden() const
{
	QtIndexingDialog* window = dynamic_cast<QtIndexingDialog*>(m_windowStack.getTopWindow());
	if (window)
	{
		return window->isHidden();
	}

	return false;
}

void QtDialogView::clearDialogs()
{
	m_onQtThread2(
		[=]()
		{
			m_windowStack.clearWindows();

			setUIBlocked(false);
		}
	);

	setParentWindow(nullptr);
}

void QtDialogView::showUnknownProgressDialog(const std::wstring& title, const std::wstring& message)
{
	MessageStatus(title + L": " + message, false, true).dispatch();

	m_onQtThread2(
		[=]()
		{
			showUnknownProgress(title, message, false);
		}
	);
}

void QtDialogView::hideUnknownProgressDialog()
{
	MessageStatus(L"", false, false).dispatch();

	m_onQtThread2(
		[=]()
		{
			hideUnknownProgress();
		}
	);

	setParentWindow(nullptr);
}

void QtDialogView::showProgressDialog(const std::wstring& title, const std::wstring& message, size_t progress)
{
	m_onQtThread(
		[=]()
		{
			bool sendStatusMessage = true;
			QtIndexingDialog* window = dynamic_cast<QtIndexingDialog*>(m_windowStack.getTopWindow());
			if (!window || window->getType() != QtIndexingDialog::DIALOG_PROGRESS)
			{
				m_windowStack.clearWindows();

				window = createWindow();
				window->setupProgress(m_dialogsHideable);
			}
			else
			{
				sendStatusMessage = (
					window->getTitle() != title ||
					window->getMessage() != message ||
					window->getProgress() != progress
				);
			}

			if (sendStatusMessage)
			{
				MessageStatus(title + L": " + message + L" [" + std::to_wstring(progress) + L"%]", false, true).dispatch();
			}

			window->updateTitle(QString::fromStdWString(title));
			window->updateMessage(QString::fromStdWString(message));
			window->updateProgress(progress);

			if (m_updateIndexingStatus)
			{
				MessageIndexingStatus(true, false, progress).dispatch();
			}

			setUIBlocked(m_dialogsVisible);
		}
	);
}

void QtDialogView::hideProgressDialog()
{
	m_onQtThread(
		[=]()
		{
			QtIndexingDialog* window = dynamic_cast<QtIndexingDialog*>(m_windowStack.getTopWindow());
			if (window && window->getType() == QtIndexingDialog::DIALOG_PROGRESS)
			{
				m_windowStack.popWindow();
			}

			MessageStatus(L"", false, false).dispatch();

			setUIBlocked(false);
		}
	);

	setParentWindow(nullptr);
}

void QtDialogView::startIndexingDialog(
	Project* project, const std::vector<RefreshMode>& enabledModes, const RefreshInfo& info)
{
	m_refreshInfos.clear();

	m_onQtThread(
		[=]()
		{
			m_windowStack.clearWindows();

			QtIndexingDialog* window = createWindow();
			window->setupStart(enabledModes);

			m_refreshInfos.emplace(info.mode, info);

			connect(window, &QtIndexingDialog::setMode,
				[=](RefreshMode refreshMode)
				{
					auto it = m_refreshInfos.find(refreshMode);
					if (it != m_refreshInfos.end())
					{
						window->updateRefreshInfo(it->second);
						return;
					}

					std::shared_ptr<QTimer> timer = std::make_shared<QTimer>();
					timer->setSingleShot(true);
					connect(timer.get(), &QTimer::timeout,
						[=]()
						{
							showUnknownProgress(L"Preparing Indexing", L"Processing Files", true);
						}
					);
					timer->start(200);

					Task::dispatch(std::make_shared<TaskLambda>(
						[=]()
						{
							RefreshInfo info = project->getRefreshInfo(refreshMode);

							m_onQtThread2(
								[=]()
								{
									m_refreshInfos.emplace(info.mode, info);
									window->updateRefreshInfo(info);

									timer->stop();
									hideUnknownProgress();
								}
							);
						}
					));
				}
			);

			connect(window, &QtIndexingDialog::startIndexing,
				[=](RefreshMode refreshMode)
				{
					RefreshInfo info = m_refreshInfos.find(refreshMode)->second;
					Task::dispatch(std::make_shared<TaskLambda>(
						[=]()
						{
							project->buildIndex(info, this);
						}
					));

					m_windowStack.clearWindows();
				}
			);

			connect(window, &QtWindow::canceled,
				[=]()
				{
					setUIBlocked(false);
				}
			);

			window->updateRefreshInfo(info);
			setUIBlocked(true);
		}
	);
}

void QtDialogView::updateIndexingDialog(
	size_t startedFileCount, size_t finishedFileCount, size_t totalFileCount, const std::vector<FilePath>& sourcePaths)
{
	m_onQtThread(
		[=]()
		{
			if (!sourcePaths.empty())
			{
				std::vector<std::wstring> stati;
				for (const FilePath& path : sourcePaths)
				{
					stati.push_back(L"[" + std::to_wstring(startedFileCount) + L"/" + std::to_wstring(totalFileCount) + L"] Indexing file: " + path.wstr());
				}
				MessageStatus(stati, false, true, m_dialogsVisible).dispatch();
			}

			QtIndexingDialog* window = dynamic_cast<QtIndexingDialog*>(m_windowStack.getTopWindow());
			if (!window)
			{
				m_windowStack.clearWindows();

				window = createWindow();
				window->setupIndexing(m_dialogsHideable);
			}

			if (window && window->getType() == QtIndexingDialog::DIALOG_INDEXING)
			{
				window->updateIndexingProgress(finishedFileCount, totalFileCount, sourcePaths.empty() ? FilePath() : sourcePaths.back());
			}
			m_mainWindow->setWindowsTaskbarProgress(float(finishedFileCount) / totalFileCount);

			if (m_updateIndexingStatus)
			{
				int progress = 0;
				if (totalFileCount)
				{
					progress = finishedFileCount * 100 / totalFileCount;
				}

				MessageIndexingStatus(true, false, progress).dispatch();
			}

			setUIBlocked(m_dialogsVisible);
		}
	);
}

DatabasePolicy QtDialogView::finishedIndexingDialog(
	size_t indexedFileCount, size_t totalIndexedFileCount, size_t completedFileCount, size_t totalFileCount,
	float time, ErrorCountInfo errorInfo, bool interrupted)
{
	if (m_updateIndexingStatus)
	{
		MessageIndexingStatus(false, false, 0).dispatch();
	}

	DatabasePolicy policy = DATABASE_POLICY_UNKNOWN;
	m_resultReady = false;

	m_onQtThread(
		[=, &policy]()
		{
			m_dialogsVisible = true;
			m_windowStack.clearWindows();

			QtIndexingDialog* window = createWindow();
			window->setupReport(indexedFileCount, totalIndexedFileCount, completedFileCount, totalFileCount, time, interrupted);
			window->updateErrorCount(errorInfo.total, errorInfo.fatal);
			connect(window, &QtWindow::finished,
				[this, &policy]()
				{
					setUIBlocked(false);
					policy = DATABASE_POLICY_KEEP;
					m_resultReady = true;
				}
			);
			connect(window, &QtWindow::canceled,
				[this, &policy]()
				{
					setUIBlocked(false);
					policy = DATABASE_POLICY_DISCARD;
					m_resultReady = true;
				}
			);

			m_mainWindow->hideWindowsTaskbarProgress();
			setUIBlocked(true);
		}
	);

	while (!m_resultReady)
	{
		const int SLEEP_TIME_MS = 25;
		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));
	}

	return policy;
}

int QtDialogView::confirm(const std::string& message, const std::vector<std::string>& options)
{
	int result = -1;
	m_resultReady = false;

	m_onQtThread2(
		[=, &result]()
		{
			QMessageBox msgBox;
			msgBox.setText(QString::fromStdString(message));

			for (const std::string& option : options)
			{
				msgBox.addButton(QString::fromStdString(option), QMessageBox::AcceptRole);
			}

			msgBox.exec();

			for (int i = 0; i < msgBox.buttons().size(); i++)
			{
				if (msgBox.clickedButton() == msgBox.buttons().at(i))
				{
					result = i;
					break;
				}
			}

			m_resultReady = true;
		}
	);

	while (!m_resultReady)
	{
		const int SLEEP_TIME_MS = 25;
		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));
	}

	return result;
}

int QtDialogView::confirm(const std::wstring& message, const std::vector<std::wstring>& options)
{
	int result = -1;
	m_resultReady = false;

	m_onQtThread2(
		[=, &result]()
		{
			QMessageBox msgBox;
			msgBox.setText(QString::fromStdWString(message));

			for (const std::wstring& option : options)
			{
				msgBox.addButton(QString::fromStdWString(option), QMessageBox::AcceptRole);
			}

			msgBox.exec();

			for (int i = 0; i < msgBox.buttons().size(); i++)
			{
				if (msgBox.clickedButton() == msgBox.buttons().at(i))
				{
					result = i;
					break;
				}
			}

			m_resultReady = true;
		}
	);

	while (!m_resultReady)
	{
		const int SLEEP_TIME_MS = 25;
		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));
	}

	return result;
}

void QtDialogView::setParentWindow(QtWindow* window)
{
	m_onQtThread(
		[=]()
		{
			m_parentWindow = window;
		}
	);
}

void QtDialogView::showUnknownProgress(const std::wstring& title, const std::wstring& message, bool stacked)
{
	QtIndexingDialog* window = nullptr;

	if (!stacked)
	{
		window = dynamic_cast<QtIndexingDialog*>(m_windowStack.getTopWindow());

		if (window && window->getType() != QtIndexingDialog::DIALOG_UNKNOWN_PROGRESS)
		{
			m_windowStack.clearWindows();
			window = nullptr;
		}
	}

	if (!window)
	{
		window = createWindow();
		window->setupUnknownProgress(m_dialogsHideable);
	}

	window->updateTitle(QString::fromStdWString(title));
	window->updateMessage(QString::fromStdWString(message));

	if (m_updateIndexingStatus)
	{
		MessageIndexingStatus(true, true, 0).dispatch();
	}

	setUIBlocked(m_dialogsVisible);
}

void QtDialogView::hideUnknownProgress()
{
	QtIndexingDialog* window = dynamic_cast<QtIndexingDialog*>(m_windowStack.getTopWindow());
	if (window && window->getType() == QtIndexingDialog::DIALOG_UNKNOWN_PROGRESS)
	{
		m_windowStack.popWindow();
	}

	if (!m_windowStack.getWindowCount())
	{
		setUIBlocked(false);
	}
}

void QtDialogView::setUIBlocked(bool blocked)
{
	if (m_uiBlocked == blocked)
	{
		return;
	}

	m_uiBlocked = blocked;

	if (m_parentWindow)
	{
		m_parentWindow->setEnabled(!blocked);
	}
	else
	{
		m_mainWindow->setContentEnabled(!blocked);
	}

	if (blocked)
	{
		QWidget* window = m_windowStack.getTopWindow();
		if (window)
		{
			window->setEnabled(true);
		}
	}
}

void QtDialogView::dialogVisibilityChanged(bool visible)
{
	QtIndexingDialog* window = dynamic_cast<QtIndexingDialog*>(m_windowStack.getTopWindow());
	if (!window)
	{
		return;
	}

	window->setVisible(visible);
	m_dialogsVisible = visible;
	setUIBlocked(visible);

	if (!visible)
	{
		MessageStatus(L"", false, false).dispatch();
	}
}

void QtDialogView::handleMessage(MessageIndexingShowDialog* message)
{
	m_onQtThread3(
		[=]()
		{
			dialogVisibilityChanged(true);
		}
	);
}

void QtDialogView::handleMessage(MessageInterruptTasks* message)
{
	if (!m_dialogsVisible)
	{
		return;
	}

	m_onQtThread3(
		[=]()
		{
			QtIndexingDialog* window = dynamic_cast<QtIndexingDialog*>(m_windowStack.getTopWindow());
			if (window && window->getType() == QtIndexingDialog::DIALOG_INDEXING)
			{
				showUnknownProgressDialog(L"Interrupting Indexing", L"Waiting for indexer\nthreads to finish");
			}
		}
	);
}

void QtDialogView::handleMessage(MessageErrorCountUpdate* message)
{
	ErrorCountInfo errorInfo = message->errorCount;

	m_onQtThread3(
		[=]()
		{
			updateErrorCount(errorInfo.total, errorInfo.fatal);
		}
	);
}

void QtDialogView::handleMessage(MessageWindowClosed* message)
{
	m_resultReady = true;
}

void QtDialogView::updateErrorCount(size_t errorCount, size_t fatalCount)
{
	if (QtIndexingDialog* window = dynamic_cast<QtIndexingDialog*>(m_windowStack.getTopWindow()))
	{
		window->updateErrorCount(errorCount, fatalCount);
	}
}

QtIndexingDialog* QtDialogView::createWindow()
{
	QtIndexingDialog* window = nullptr;
	if (m_parentWindow)
	{
		window = new QtIndexingDialog(m_parentWindow);
	}
	else
	{
		window = new QtIndexingDialog(m_mainWindow);
	}

	connect(window, &QtWindow::canceled, &m_windowStack, &QtWindowStack::popWindow);
	connect(window, &QtWindow::finished, &m_windowStack, &QtWindowStack::clearWindows);

	connect(window, &QtIndexingDialog::visibleChanged, this, &QtDialogView::dialogVisibilityChanged);

	m_windowStack.pushWindow(window);

	if (!m_dialogsVisible)
	{
		window->hide();
	}

	return window;
}
