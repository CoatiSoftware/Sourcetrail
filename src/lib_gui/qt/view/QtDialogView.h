#ifndef QT_DIALOG_VIEW_H
#define QT_DIALOG_VIEW_H

#include "component/view/DialogView.h"

#include "qt/utility/QtThreadedFunctor.h"
#include "qt/window/QtWindowStack.h"

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/error/MessageErrorCountUpdate.h"
#include "utility/messaging/type/MessageInterruptTasks.h"
#include "utility/messaging/type/MessageWindowClosed.h"

class QtMainWindow;
class QtWindow;

class QtDialogView
	: public QObject
	, public DialogView
	, public MessageListener<MessageErrorCountUpdate>
	, public MessageListener<MessageInterruptTasks>
	, public MessageListener<MessageWindowClosed>
{
	Q_OBJECT

public:
	QtDialogView(QtMainWindow* mainWindow, StorageAccess* storageAccess);
	~QtDialogView() override;

	void showUnknownProgressDialog(const std::wstring& title, const std::wstring& message) override;
	void hideUnknownProgressDialog() override;

	void showProgressDialog(const std::wstring& title, const std::wstring& message, size_t progress) override;
	void hideProgressDialog() override;

	void startIndexingDialog(
		Project* project, const std::vector<RefreshMode>& enabledModes, const RefreshInfo& info) override;
	void updateIndexingDialog(
		size_t startedFileCount, size_t finishedFileCount, size_t totalFileCount, const FilePath& sourcePath) override;
	DatabasePolicy finishedIndexingDialog(
		size_t indexedFileCount, size_t totalIndexedFileCount, size_t completedFileCount, size_t totalFileCount,
		float time, ErrorCountInfo errorInfo, bool interrupted) override;

	void hideDialogs(bool unblockUI = true) override;

	int confirm(const std::string& message, const std::vector<std::string>& options) override;
	int confirm(const std::wstring& message, const std::vector<std::wstring>& options) override;

	void setParentWindow(QtWindow* window);

private slots:
	void showUnknownProgress(const std::wstring& title, const std::wstring& message, bool stacked);
	void hideUnknownProgress();

	void setUIBlocked(bool blocked);

private:
	void handleMessage(MessageErrorCountUpdate* message) override;
	void handleMessage(MessageInterruptTasks* message) override;
	void handleMessage(MessageWindowClosed* message) override;

	void updateErrorCount(size_t errorCount, size_t fatalCount);

	template<typename T>
		T* createWindow();

	QtMainWindow* m_mainWindow;
	QtWindow* m_parentWindow;

	QtWindowStack m_windowStack;

	QtThreadedLambdaFunctor m_onQtThread;
	QtThreadedLambdaFunctor m_onQtThread2;
	QtThreadedLambdaFunctor m_onQtThread3;

	std::map<RefreshMode, RefreshInfo> m_refreshInfos;

	bool m_resultReady;
};

#endif // QT_DIALOG_VIEW_H
