#include "qt/view/QtDialogView.h"

#include <chrono>
#include <sstream>
#include <thread>

#include <QMessageBox>

#include "qt/window/QtIndexingDialog.h"
#include "qt/window/QtMainWindow.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/utility.h"

QtDialogView::QtDialogView(QtMainWindow* mainWindow)
	: m_mainWindow(mainWindow)
	, m_windowStack(this)
{
}

QtDialogView::~QtDialogView()
{
}

void QtDialogView::showProgressDialog(const std::string& title, const std::string& message)
{
	MessageStatus(title + ": " + message, false, true).dispatch();

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
}

bool QtDialogView::startIndexingDialog(size_t cleanFileCount, size_t indexFileCount)
{
	bool result = false;
	bool done = false;

	m_onQtThread(
		[=, &result, &done]()
		{
			QtIndexingDialog* window = createWindow<QtIndexingDialog>();
			window->setupStart(cleanFileCount, indexFileCount,
				[&](bool start)
				{
					result = start;
					done = true;

					setUIBlocked(false);
				}
			);

			setUIBlocked(true);
		}
	);

	while (!done)
	{
		const int SLEEP_TIME_MS = 25;
		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));
	}

	return result;
}

void QtDialogView::updateIndexingDialog(size_t fileCount, size_t totalFileCount, std::string sourcePath)
{
	std::stringstream ss;
	ss << "Indexing files: [";
	ss << fileCount << "/";
	ss << totalFileCount << "] ";
	ss << sourcePath;
	MessageStatus(ss.str(), false, true).dispatch();

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
				window->updateIndexingProgress(fileCount, totalFileCount, sourcePath);
				setUIBlocked(true);
			}
		}
	);
}

void QtDialogView::finishedIndexingDialog(size_t fileCount, size_t totalFileCount, float time, ErrorCountInfo errorInfo)
{
	std::stringstream ss;
	ss << "Finished indexing: ";
	ss << fileCount << "/" << totalFileCount << " files; ";
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
			window->setupReport(fileCount, totalFileCount, time);
			window->updateErrorCount(errorInfo.total, errorInfo.fatal);

			setUIBlocked(false);
		}
	);
}

int QtDialogView::confirm(const std::string& message, const std::vector<std::string>& options)
{
	int result = -1;
	bool done = false;

	m_onQtThread(
		[=, &result, &done]()
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

			done = true;
		}
	);

	while (!done)
	{
		const int SLEEP_TIME_MS = 25;
		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));
	}

	return result;
}

void QtDialogView::handleMessage(MessageInterruptTasks* message)
{
	m_onQtThread2(
		[=]()
		{
			QtIndexingDialog* window = dynamic_cast<QtIndexingDialog*>(m_windowStack.getTopWindow());
			if (window && window->getType() == QtIndexingDialog::DIALOG_INDEXING)
			{
				showProgressDialog("Interrupting Indexing", "Waiting for indexer\nthreads to finish");
			}
		}
	);
}

void QtDialogView::handleMessage(MessageShowErrors* message)
{
	ErrorCountInfo errorInfo = message->errorCount;

	m_onQtThread2(
		[=]()
		{
			updateErrorCount(errorInfo.total, errorInfo.fatal);
		}
	);
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
	T* window = new T(m_mainWindow);

	connect(window, SIGNAL(canceled()), &m_windowStack, SLOT(popWindow()));
	connect(window, SIGNAL(finished()), &m_windowStack, SLOT(clearWindows()));

	m_windowStack.pushWindow(window);

	return window;
}

void QtDialogView::setUIBlocked(bool blocked)
{
	m_mainWindow->setEnabled(!blocked);

	if (blocked)
	{
		QWidget* window = m_windowStack.getTopWindow();
		if (window)
		{
			window->setEnabled(true);
		}
	}
}
