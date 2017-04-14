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

	virtual void showStatusDialog(const std::string& title, const std::string& message) override;
	virtual void hideStatusDialog() override;

	virtual void showProgressDialog(const std::string& title, const std::string& message, int progress) override;
	virtual void hideProgressDialog() override;

	virtual DialogView::IndexMode startIndexingDialog(size_t cleanFileCount, size_t indexFileCount, size_t totalFileCount,
		bool forceRefresh, bool needsFullRefresh) override;
	virtual void updateIndexingDialog(size_t fileCount, size_t totalFileCount, std::string sourcePath) override;
	virtual void finishedIndexingDialog(
		size_t indexedFileCount, size_t totalIndexedFileCount, size_t completedFileCount, size_t totalFileCount,
		float time, ErrorCountInfo errorInfo) override;

	int confirm(const std::string& message, const std::vector<std::string>& options) override;

private:
	void handleMessage(MessageInterruptTasks* message) override;
	void handleMessage(MessageNewErrors* message) override;
	void handleMessage(MessageShowErrors* message) override;
	void handleMessage(MessageWindowClosed* message) override;

	void updateErrorCount(size_t errorCount, size_t fatalCount);

	template<typename T>
		T* createWindow();

	void setUIBlocked(bool blocked);

	QtMainWindow* m_mainWindow;

	QtWindowStack m_windowStack;

	QtThreadedLambdaFunctor m_onQtThread;
	QtThreadedLambdaFunctor m_onQtThread2;

	bool m_resultReady;
};

#endif // QT_DIALOG_VIEW_H
