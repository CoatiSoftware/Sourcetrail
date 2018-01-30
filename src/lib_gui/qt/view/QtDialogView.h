#ifndef QT_DIALOG_VIEW_H
#define QT_DIALOG_VIEW_H

#include "component/view/DialogView.h"

#include "qt/utility/QtThreadedFunctor.h"
#include "qt/window/QtWindowStack.h"

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageInterruptTasks.h"
#include "utility/messaging/type/MessageNewErrors.h"
#include "utility/messaging/type/MessageShowErrors.h"
#include "utility/messaging/type/MessageWindowClosed.h"

class QtMainWindow;
class QtWindow;

class QtDialogView
	: public QObject
	, public DialogView
	, public MessageListener<MessageInterruptTasks>
	, public MessageListener<MessageNewErrors>
	, public MessageListener<MessageShowErrors>
	, public MessageListener<MessageWindowClosed>
{
	Q_OBJECT

public:
	QtDialogView(QtMainWindow* mainWindow, StorageAccess* storageAccess);
	virtual ~QtDialogView();

	virtual void showUnknownProgressDialog(const std::string& title, const std::string& message) override;
	virtual void hideUnknownProgressDialog() override;

	virtual void showProgressDialog(const std::string& title, const std::string& message, size_t progress) override;
	virtual void hideProgressDialog() override;

	virtual void startIndexingDialog(
		Project* project, const std::vector<RefreshMode>& enabledModes, const RefreshInfo& info) override;
	virtual void updateIndexingDialog(
		size_t startedFileCount, size_t finishedFileCount, size_t totalFileCount, std::string sourcePath) override;
	virtual void finishedIndexingDialog(
		size_t indexedFileCount, size_t totalIndexedFileCount, size_t completedFileCount, size_t totalFileCount,
		float time, ErrorCountInfo errorInfo, bool interrupted) override;

	virtual void hideDialogs(bool unblockUI = true) override;

	virtual int confirm(const std::string& message, const std::vector<std::string>& options) override;
	virtual int confirm(const std::wstring& message, const std::vector<std::wstring>& options) override;

	void setParentWindow(QtWindow* window);

private slots:
	void showUnknownProgress(const std::string& title, const std::string& message, bool stacked);
	void hideUnknownProgress();

	void setUIBlocked(bool blocked);

private:
	void handleMessage(MessageInterruptTasks* message) override;
	void handleMessage(MessageNewErrors* message) override;
	void handleMessage(MessageShowErrors* message) override;
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
