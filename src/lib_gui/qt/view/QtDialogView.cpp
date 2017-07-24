#include "qt/view/QtDialogView.h"

#include <chrono>
#include <sstream>
#include <thread>

#include <QMessageBox>

#include "data/access/StorageAccess.h"
#include "qt/window/QtIndexingDialog.h"
#include "qt/window/QtMainWindow.h"
#include "qt/window/QtWindow.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/utility.h"

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

	m_onQtThread(
		[=]()
		{
			QtIndexingDialog* window = dynamic_cast<QtIndexingDialog*>(m_windowStack.getTopWindow());
			if (!window || window->getType() != QtIndexingDialog::DIALOG_UNKNOWN_PROGRESS)
			{
				m_windowStack.clearWindows();

				window = createWindow<QtIndexingDialog>();
				window->setupUnknownProgress();
			}

			window->updateTitle(title.c_str());
			window->updateMessage(message.c_str());

			setUIBlocked(true);
		}
	);
}

void QtDialogView::hideUnknownProgressDialog()
{
	MessageStatus("", false, false).dispatch();

	m_onQtThread(
		[=]()
		{
			QtIndexingDialog* window = dynamic_cast<QtIndexingDialog*>(m_windowStack.getTopWindow());
			if (window && window->getType() == QtIndexingDialog::DIALOG_UNKNOWN_PROGRESS)
			{
				m_windowStack.popWindow();
			}

			setUIBlocked(false);
		}
	);

	setParentWindow(nullptr);
}

void QtDialogView::showProgressDialog(const std::string& title, const std::string& message, int progress)
{
	MessageStatus(title + ": " + message + " [" + std::to_string(progress) + "%]", false, true).dispatch();

	m_onQtThread(
		[=]()
		{
			QtIndexingDialog* window = dynamic_cast<QtIndexingDialog*>(m_windowStack.getTopWindow());
			if (!window || window->getType() != QtIndexingDialog::DIALOG_PROGRESS)
			{
				m_windowStack.clearWindows();

				window = createWindow<QtIndexingDialog>();
				window->setupProgress();
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
	MessageStatus("", false, false).dispatch();

	m_onQtThread(
		[=]()
		{
			QtIndexingDialog* window = dynamic_cast<QtIndexingDialog*>(m_windowStack.getTopWindow());
			if (window && window->getType() == QtIndexingDialog::DIALOG_PROGRESS)
			{
				m_windowStack.popWindow();
			}

			setUIBlocked(false);
		}
	);

	setParentWindow(nullptr);
}


DialogView::IndexingOptions QtDialogView::startIndexingDialog(
	size_t cleanFileCount, size_t indexFileCount, size_t totalFileCount, DialogView::IndexingOptions options)
{
	DialogView::IndexingOptions result;
	m_resultReady = false;

	m_onQtThread(
		[=, &result]()
		{
			QtIndexingDialog* window = createWindow<QtIndexingDialog>();
			window->setupStart(cleanFileCount, indexFileCount, totalFileCount, options,
				[&](DialogView::IndexingOptions o)
				{
					result = o;
					m_resultReady = true;

					setUIBlocked(o.startIndexing);
				}
			);

			setUIBlocked(true);
		}
	);

	while (!m_resultReady)
	{
		const int SLEEP_TIME_MS = 25;
		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));
	}

	return result;
}

void QtDialogView::updateIndexingDialog(
	size_t startedFileCount, size_t finishedFileCount, size_t totalFileCount, std::string sourcePath)
{
	if (sourcePath.size())
	{
		std::stringstream ss;
		ss << "Indexing file: [";
		ss << startedFileCount << "/";
		ss << totalFileCount << "] ";
		ss << sourcePath;
		MessageStatus(ss.str(), false, true).dispatch();
	}

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
		}
	);
}

void QtDialogView::finishedIndexingDialog(
	size_t indexedFileCount, size_t totalIndexedFileCount, size_t completedFileCount, size_t totalFileCount,
	float time, ErrorCountInfo errorInfo, bool interrupted)
{
	std::stringstream ss;
	ss << "Finished indexing: ";
	ss << indexedFileCount << "/" << totalIndexedFileCount << " source files indexed; ";
	ss << utility::timeToString(time);
	ss << "; " << errorInfo.total << " error" << (errorInfo.total != 1 ? "s" : "");
	if (errorInfo.fatal > 0)
	{
		ss << " (" << errorInfo.fatal << " fatal)";
	}
	MessageStatus(ss.str(), false, false).dispatch();

	m_onQtThread(
		[=]()
		{
			m_windowStack.clearWindows();

			QtIndexingDialog* window = createWindow<QtIndexingDialog>();
			window->setupReport(indexedFileCount, totalIndexedFileCount, completedFileCount, totalFileCount, time, interrupted);
			window->updateErrorCount(errorInfo.total, errorInfo.fatal);

			setUIBlocked(false);
		}
	);
}

void QtDialogView::hideDialogs()
{
	m_onQtThread(
		[=]()
		{
			m_windowStack.clearWindows();
			setUIBlocked(false);
		}
	);

	setParentWindow(nullptr);
}

int QtDialogView::confirm(const std::string& message, const std::vector<std::string>& options)
{
	int result = -1;
	m_resultReady = false;

	m_onQtThread(
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

void QtDialogView::handleMessage(MessageInterruptTasks* message)
{
	m_onQtThread2(
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

	m_onQtThread2(
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

	m_onQtThread2(
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
	QtIndexingDialog* window = dynamic_cast<QtIndexingDialog*>(m_windowStack.getTopWindow());
	if (window)
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

	connect(window, SIGNAL(canceled()), &m_windowStack, SLOT(popWindow()));
	connect(window, SIGNAL(finished()), &m_windowStack, SLOT(clearWindows()));

	m_windowStack.pushWindow(window);

	return window;
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
