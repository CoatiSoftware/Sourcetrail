#include "QtDialogView.h"

#include <chrono>
#include <sstream>
#include <thread>

#include <QMessageBox>
#include <QTimer>

#include "MessageIndexingStatus.h"
#include "MessageStatus.h"
#include "Project.h"
#include "QtIndexingDialog.h"
#include "QtIndexingProgressDialog.h"
#include "QtIndexingReportDialog.h"
#include "QtIndexingStartDialog.h"
#include "QtKnownProgressDialog.h"
#include "QtMainWindow.h"
#include "QtUnknownProgressDialog.h"
#include "QtWindow.h"
#include "StorageAccess.h"
#include "TabId.h"
#include "TaskLambda.h"
#include "utility.h"

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
	return !m_dialogsVisible;
}

void QtDialogView::clearDialogs()
{
	m_onQtThread2([=]() {
		m_windowStack.clearWindows();

		setUIBlocked(false);
	});

	setParentWindow(nullptr);
}

void QtDialogView::showUnknownProgressDialog(const std::wstring& title, const std::wstring& message)
{
	MessageStatus(title + L": " + message, false, true).dispatch();

	m_onQtThread2([=]() { showUnknownProgress(title, message, false); });
}

void QtDialogView::hideUnknownProgressDialog()
{
	MessageStatus(L"", false, false).dispatch();

	m_onQtThread2([=]() { hideUnknownProgress(); });

	setParentWindow(nullptr);
}

void QtDialogView::showProgressDialog(
	const std::wstring& title, const std::wstring& message, size_t progress)
{
	m_onQtThread([=]() {
		bool sendStatusMessage = true;
		QtKnownProgressDialog* window = dynamic_cast<QtKnownProgressDialog*>(
			m_windowStack.getTopWindow());
		if (!window)
		{
			m_windowStack.clearWindows();
			window = createWindow<QtKnownProgressDialog>(m_dialogsHideable);
		}
		else
		{
			sendStatusMessage =
				(window->getTitle() != title || window->getMessage() != message ||
				 window->getProgress() != progress);
		}

		if (sendStatusMessage)
		{
			MessageStatus(
				title + L": " + message + L" [" + std::to_wstring(progress) + L"%]", false, true)
				.dispatch();
		}

		window->updateTitle(QString::fromStdWString(title));
		window->updateMessage(QString::fromStdWString(message));
		window->updateProgress(progress);

		setUIBlocked(m_dialogsVisible);
	});
}

void QtDialogView::hideProgressDialog()
{
	m_onQtThread([=]() {
		QtKnownProgressDialog* window = dynamic_cast<QtKnownProgressDialog*>(
			m_windowStack.getTopWindow());
		if (window)
		{
			m_windowStack.popWindow();
		}

		MessageStatus(L"", false, false).dispatch();

		setUIBlocked(false);
	});

	setParentWindow(nullptr);
}

void QtDialogView::startIndexingDialog(
	Project* project,
	const std::vector<RefreshMode>& enabledModes,
	const RefreshMode initialMode,
	bool enabledShallowOption,
	bool initialShallowState,
	std::function<void(const RefreshInfo& info)> onStartIndexing,
	std::function<void()> onCancelIndexing)
{
	m_refreshInfos.clear();
	m_shallowIndexingEnabled = initialShallowState;

	m_onQtThread([=]() {
		m_dialogsVisible = true;
		m_windowStack.clearWindows();

		QtIndexingStartDialog* window = createWindow<QtIndexingStartDialog>(
			enabledModes, initialMode, enabledShallowOption, initialShallowState);

		connect(window, &QtIndexingStartDialog::setShallowIndexing, [=](bool enabled) {
			m_shallowIndexingEnabled = enabled;
		});

		std::function<void(RefreshMode)> onRefreshModeChanged = ([=](RefreshMode refreshMode) {
			auto it = m_refreshInfos.find(refreshMode);
			if (it != m_refreshInfos.end())
			{
				window->updateRefreshInfo(it->second);
				window->show();
				return;
			}

			std::shared_ptr<QTimer> timer = std::make_shared<QTimer>();
			timer->setSingleShot(true);
			connect(timer.get(), &QTimer::timeout, [=]() {
				showUnknownProgress(L"Preparing Indexing", L"Processing Files", true);
			});
			timer->start(200);

			Task::dispatch(TabId::app(), std::make_shared<TaskLambda>([=]() {
							   RefreshInfo info = project->getRefreshInfo(refreshMode);

							   m_onQtThread2([=]() {
								   m_dialogsVisible = true;
								   m_refreshInfos.emplace(info.mode, info);
								   window->updateRefreshInfo(info);

								   timer->stop();
								   hideUnknownProgress();
								   window->show();
							   });
						   }));
		});

		connect(window, &QtIndexingStartDialog::setMode, onRefreshModeChanged);

		connect(window, &QtIndexingStartDialog::startIndexing, [=](RefreshMode refreshMode) {
			RefreshInfo info = m_refreshInfos.find(refreshMode)->second;
			info.shallow = m_shallowIndexingEnabled;
			Task::dispatch(
				TabId::app(), std::make_shared<TaskLambda>([=]() { onStartIndexing(info); }));

			m_windowStack.clearWindows();
		});

		connect(window, &QtIndexingDialog::canceled, [=]() {
			Task::dispatch(TabId::app(), std::make_shared<TaskLambda>([=]() { onCancelIndexing(); }));

			setUIBlocked(false);
		});

		onRefreshModeChanged(initialMode);

		window->hide();
		setUIBlocked(true);
	});
}

void QtDialogView::updateIndexingDialog(
	size_t startedFileCount,
	size_t finishedFileCount,
	size_t totalFileCount,
	const std::vector<FilePath>& sourcePaths)
{
	m_onQtThread([=]() {
		if (!sourcePaths.empty())
		{
			std::vector<std::wstring> stati;
			for (const FilePath& path: sourcePaths)
			{
				stati.push_back(
					L"[" + std::to_wstring(startedFileCount) + L"/" +
					std::to_wstring(totalFileCount) + L"] Indexing file: " + path.wstr());
			}
			MessageStatus(stati, false, false, m_dialogsVisible).dispatch();
		}

		QtIndexingProgressDialog* window = dynamic_cast<QtIndexingProgressDialog*>(
			m_windowStack.getTopWindow());
		if (!window)
		{
			m_windowStack.clearWindows();

			window = createWindow<QtIndexingProgressDialog>(m_dialogsHideable);
		}

		window->updateIndexingProgress(
			finishedFileCount, totalFileCount, sourcePaths.empty() ? FilePath() : sourcePaths.back());

		m_mainWindow->setWindowsTaskbarProgress(float(finishedFileCount) / totalFileCount);

		setUIBlocked(m_dialogsVisible);
	});
}

void QtDialogView::updateCustomIndexingDialog(
	size_t startedFileCount,
	size_t finishedFileCount,
	size_t totalFileCount,
	const std::vector<FilePath>& sourcePaths)
{
	updateIndexingDialog(startedFileCount, finishedFileCount, totalFileCount, sourcePaths);

	m_onQtThread([=]() {
		QtIndexingProgressDialog* window = dynamic_cast<QtIndexingProgressDialog*>(
			m_windowStack.getTopWindow());
		if (window)
		{
			window->updateTitle(QStringLiteral("Executing Commands"));
		}
	});
}

DatabasePolicy QtDialogView::finishedIndexingDialog(
	size_t indexedFileCount,
	size_t totalIndexedFileCount,
	size_t completedFileCount,
	size_t totalFileCount,
	float time,
	ErrorCountInfo errorInfo,
	bool interrupted,
	bool shallow)
{
	DatabasePolicy policy = DATABASE_POLICY_UNKNOWN;
	m_resultReady = false;

	m_onQtThread([=, &policy]() {
		m_dialogsVisible = true;
		m_windowStack.clearWindows();

		QtIndexingReportDialog* window = createWindow<QtIndexingReportDialog>(
			indexedFileCount,
			totalIndexedFileCount,
			completedFileCount,
			totalFileCount,
			time,
			interrupted,
			shallow);
		window->updateErrorCount(errorInfo.total, errorInfo.fatal);
		connect(window, &QtIndexingDialog::finished, [this, &policy]() {
			setUIBlocked(false);
			policy = DATABASE_POLICY_KEEP;
			m_resultReady = true;
		});
		connect(window, &QtIndexingDialog::canceled, [this, &policy]() {
			setUIBlocked(false);
			policy = DATABASE_POLICY_DISCARD;
			m_resultReady = true;
		});
		connect(window, &QtIndexingReportDialog::requestReindexing, [this, &policy]() {
			setUIBlocked(false);
			policy = DATABASE_POLICY_REFRESH;
			m_resultReady = true;
		});

		m_mainWindow->hideWindowsTaskbarProgress();
		setUIBlocked(true);
	});

	while (!m_resultReady)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}

	return policy;
}

int QtDialogView::confirm(const std::wstring& message, const std::vector<std::wstring>& options)
{
	int result = -1;
	m_resultReady = false;

	m_onQtThread2([=, &result]() {
		QMessageBox msgBox;
		msgBox.setText(QString::fromStdWString(message));

		for (const std::wstring& option: options)
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
	});

	while (!m_resultReady)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}

	return result;
}

void QtDialogView::setParentWindow(QtWindow* window)
{
	m_onQtThread([=]() { m_parentWindow = window; });
}

void QtDialogView::showUnknownProgress(const std::wstring& title, const std::wstring& message, bool stacked)
{
	QtUnknownProgressDialog* window = nullptr;

	if (!stacked)
	{
		window = dynamic_cast<QtUnknownProgressDialog*>(m_windowStack.getTopWindow());

		if (!window)
		{
			m_windowStack.clearWindows();
			window = nullptr;
		}
	}

	if (!window)
	{
		window = createWindow<QtUnknownProgressDialog>(m_dialogsHideable);
	}

	window->updateTitle(QString::fromStdWString(title));
	window->updateMessage(QString::fromStdWString(message));

	setUIBlocked(m_dialogsVisible);
}

void QtDialogView::hideUnknownProgress()
{
	QtUnknownProgressDialog* window = dynamic_cast<QtUnknownProgressDialog*>(
		m_windowStack.getTopWindow());
	if (window)
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
	QtWindowStackElement* window = m_windowStack.getTopWindow();
	if (!window)
	{
		return;
	}

	window->setVisible(visible);
	m_dialogsVisible = visible;
	setUIBlocked(visible);

	if (!visible && dynamic_cast<QtIndexingDialog*>(window))
	{
		MessageStatus(L"", false, false).dispatch();
	}
}

void QtDialogView::handleMessage(MessageIndexingShowDialog* message)
{
	m_onQtThread3([=]() { dialogVisibilityChanged(true); });
}

void QtDialogView::handleMessage(MessageErrorCountUpdate* message)
{
	ErrorCountInfo errorInfo = message->errorCount;

	m_onQtThread3([=]() { updateErrorCount(errorInfo.total, errorInfo.fatal); });
}

void QtDialogView::handleMessage(MessageWindowClosed* message)
{
	m_resultReady = true;
}

void QtDialogView::updateErrorCount(size_t errorCount, size_t fatalCount)
{
	if (QtIndexingProgressDialog* progressWindow = dynamic_cast<QtIndexingProgressDialog*>(
			m_windowStack.getTopWindow()))
	{
		progressWindow->updateErrorCount(errorCount, fatalCount);
	}
	else if (
		QtIndexingReportDialog* reportWindow = dynamic_cast<QtIndexingReportDialog*>(
			m_windowStack.getTopWindow()))
	{
		reportWindow->updateErrorCount(errorCount, fatalCount);
	}
}

template <typename DialogType, typename... ParamTypes>
DialogType* QtDialogView::createWindow(ParamTypes... params)
{
	DialogType* window = nullptr;
	if (m_parentWindow)
	{
		window = new DialogType(params..., m_parentWindow);
	}
	else
	{
		window = new DialogType(params..., m_mainWindow);
	}

	connect(window, &QtIndexingDialog::canceled, &m_windowStack, &QtWindowStack::popWindow);
	connect(window, &QtIndexingDialog::finished, &m_windowStack, &QtWindowStack::clearWindows);
	connect(window, &QtIndexingDialog::visibleChanged, this, &QtDialogView::dialogVisibilityChanged);

	if (m_mainWindow)
	{
		connect(
			m_mainWindow,
			&QtMainWindow::hideIndexingDialog,
			std::bind(&QtDialogView::dialogVisibilityChanged, this, true));
	}

	m_windowStack.pushWindow(window);

	if (!m_dialogsVisible)
	{
		window->hide();
	}

	return window;
}
