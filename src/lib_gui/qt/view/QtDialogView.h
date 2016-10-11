#ifndef QT_DIALOG_VIEW_H
#define QT_DIALOG_VIEW_H

#include "component/view/DialogView.h"

#include "qt/utility/QtThreadedFunctor.h"
#include "qt/window/QtWindowStack.h"

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageInterruptTasks.h"
#include "utility/messaging/type/MessageShowErrors.h"
#include "utility/messaging/type/MessageWindowClosed.h"

class QtMainWindow;

class QtDialogView
	: public QObject
	, public DialogView
	, public MessageListener<MessageInterruptTasks>
	, public MessageListener<MessageShowErrors>
	, public MessageListener<MessageWindowClosed>
{
	Q_OBJECT

public:
	QtDialogView(QtMainWindow* mainWindow);
	virtual ~QtDialogView();

	void showProgressDialog(const std::string& title, const std::string& message) override;
	void hideProgressDialog() override;

	bool startIndexingDialog(size_t cleanFileCount, size_t indexFileCount) override;
	void updateIndexingDialog(size_t fileCount, size_t totalFileCount, std::string sourcePath) override;
	void finishedIndexingDialog(size_t fileCount, size_t totalFileCount, float time, ErrorCountInfo errorInfo) override;

	int confirm(const std::string& message, const std::vector<std::string>& options) override;

private:
	void handleMessage(MessageInterruptTasks* message) override;
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
