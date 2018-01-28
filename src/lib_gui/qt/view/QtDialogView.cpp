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
#include "utility/messaging/type/MessageStatus.h"
#include "utility/scheduling/TaskLambda.h"
#include "utility/utility.h"
#include "project/Project.h"

QtDialogView::QtDialogView(QtMainWindow* mainWindow, StorageAccess* storageAccess)
	: DialogView(storageAccess)
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

void QtDialogView::showUnknownProgressDialog(const std::string& title, const std::string& message)
{
	MessageStatus(title + ": " + message, false, true).dispatch();

	m_onQtThread2(
		[=]()
		{
			showUnknownProgress(title, message, false);
		}
	);
}

void QtDialogView::hideUnknownProgressDialog()
{
	MessageStatus("", false, false).dispatch();

	m_onQtThread2(
		[=]()
		{
			hideUnknownProgress();
		}
	);

	setParentWindow(nullptr);
}

void QtDialogView::showProgressDialog(const std::string& title, const std::string& message, size_t progress)
{
	m_onQtThread(
		[=]()
		{
			bool sendStatusMessage = true;
			QtIndexingDialog* window = dynamic_cast<QtIndexingDialog*>(m_windowStack.getTopWindow());
			if (!window || window->getType() != QtIndexingDialog::DIALOG_PROGRESS)
			{
				m_windowStack.clearWindows();

				window = createWindow<QtIndexingDialog>();
				window->setupProgress();
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
				MessageStatus(title + ": " + message + " [" + std::to_string(progress) + "%]", false, true).dispatch();
			}

			window->updateTitle(title.c_str());
			window->updateMessage(message.c_str());
			window->updateProgress(progress);

			setUIBlocked(true);
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

			MessageStatus("", false, false).dispatch();

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

			QtIndexingDialog* window = createWindow<QtIndexingDialog>();
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
							showUnknownProgress("Preparing Indexing", "Processing Files", true);
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
	size_t startedFileCount, size_t finishedFileCount, size_t totalFileCount, std::string sourcePath)
{
	m_onQtThread(
		[=]()
		{
			QtIndexingDialog* window = dynamic_cast<QtIndexingDialog*>(m_windowStack.getTopWindow());
			if (!window)
			{
				m_windowStack.clearWindows();

				window = createWindow<QtIndexingDialog>();
				window->setupIndexing();
			}

			if (window && window->getType() == QtIndexingDialog::DIALOG_INDEXING)
			{
				window->updateIndexingProgress(finishedFileCount, totalFileCount, sourcePath);
				setUIBlocked(true);
			}
			m_mainWindow->setWindowsTaskbarProgress(float(finishedFileCount) / totalFileCount);
		}
	);
}

void QtDialogView::finishedIndexingDialog(
	size_t indexedFileCount, size_t totalIndexedFileCount, size_t completedFileCount, size_t totalFileCount,
	float time, ErrorCountInfo errorInfo, bool interrupted)
{

	m_onQtThread(
		[=]()
		{
			m_windowStack.clearWindows();

			QtIndexingDialog* window = createWindow<QtIndexingDialog>();
			window->setupReport(indexedFileCount, totalIndexedFileCount, completedFileCount, totalFileCount, time, interrupted);
			window->updateErrorCount(errorInfo.total, errorInfo.fatal);

			setUIBlocked(false);
			m_mainWindow->hideWindowsTaskbarProgress();
		}
	);
}

void QtDialogView::hideDialogs(bool unblockUI)
{
	m_onQtThread2(
		[=]()
		{
			m_windowStack.clearWindows();

			if (unblockUI)
			{
				setUIBlocked(false);
			}
		}
	);

	setParentWindow(nullptr);
}

int QtDialogView::confirm(const std::string& message, const std::vector<std::string>& options)
{
	int result = -1;
	m_resultReady = false;

	m_onQtThread2(
		[=, &result]()
		{
			QMessageBox msgBox;
			msgBox.setText(message.c_str());

			for (const std::string& option : options)
			{
				msgBox.addButton(option.c_str(), QMessageBox::AcceptRole);
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

void QtDialogView::showUnknownProgress(const std::string& title, const std::string& message, bool stacked)
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
		window = createWindow<QtIndexingDialog>();
		window->setupUnknownProgress();
	}

	window->updateTitle(title.c_str());
	window->updateMessage(message.c_str());

	setUIBlocked(true);
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

void QtDialogView::handleMessage(MessageInterruptTasks* message)
{
	m_onQtThread3(
		[=]()
		{
			QtIndexingDialog* window = dynamic_cast<QtIndexingDialog*>(m_windowStack.getTopWindow());
			if (window && window->getType() == QtIndexingDialog::DIALOG_INDEXING)
			{
				showUnknownProgressDialog("Interrupting Indexing", "Waiting for indexer\nthreads to finish");
			}
		}
	);
}

void QtDialogView::handleMessage(MessageNewErrors* message)
{
	ErrorCountInfo errorInfo = m_storageAccess->getErrorCount();

	m_onQtThread3(
		[=]()
		{
			updateErrorCount(errorInfo.total, errorInfo.fatal);
		}
	);
}

void QtDialogView::handleMessage(MessageShowErrors* message)
{
	if (message->isReplayed())
	{
		return;
	}

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

template<typename T>
	T* QtDialogView::createWindow()
{
	T* window = nullptr;
	if (m_parentWindow)
	{
		window = new T(m_parentWindow);
	}
	else
	{
		window = new T(m_mainWindow);
	}

	//make sure T is a QtWindow
	if (dynamic_cast<QtWindow*>(window))
	{
		connect(window, &QtWindow::canceled, &m_windowStack, &QtWindowStack::popWindow);
		connect(window, &QtWindow::finished, &m_windowStack, &QtWindowStack::clearWindows);

		m_windowStack.pushWindow(window);
	}

	return window;
}
