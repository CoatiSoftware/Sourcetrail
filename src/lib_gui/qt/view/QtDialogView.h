#ifndef QT_DIALOG_VIEW_H
#define QT_DIALOG_VIEW_H

#include "../../../lib/component/view/DialogView.h"

#include "../utility/QtThreadedFunctor.h"
#include "../window/QtWindowStack.h"

#include "../../../lib/utility/messaging/type/error/MessageErrorCountUpdate.h"
#include "../../../lib/utility/messaging/type/indexing/MessageIndexingShowDialog.h"
#include "../../../lib/utility/messaging/MessageListener.h"
#include "../../../lib/utility/messaging/type/MessageWindowClosed.h"

class QtMainWindow;
class QtWindow;

class QtDialogView
	: public QObject
	, public DialogView
	, public MessageListener<MessageErrorCountUpdate>
	, public MessageListener<MessageIndexingShowDialog>
	, public MessageListener<MessageWindowClosed>
{
	Q_OBJECT

public:
	QtDialogView(QtMainWindow* mainWindow, UseCase useCase, StorageAccess* storageAccess);
	~QtDialogView() override;

	bool dialogsHidden() const override;
	void clearDialogs() override;

	void showUnknownProgressDialog(const std::wstring& title, const std::wstring& message) override;
	void hideUnknownProgressDialog() override;

	void showProgressDialog(
		const std::wstring& title, const std::wstring& message, size_t progress) override;
	void hideProgressDialog() override;

	void startIndexingDialog(
		Project* project,
		const std::vector<RefreshMode>& enabledModes,
		const RefreshMode initialMode,
		bool enabledShallowOption,
		bool initialShallowState,
		std::function<void(const RefreshInfo& info)> onStartIndexing,
		std::function<void()> onCancelIndexing) override;
	void updateIndexingDialog(
		size_t startedFileCount,
		size_t finishedFileCount,
		size_t totalFileCount,
		const std::vector<FilePath>& sourcePaths) override;
	void updateCustomIndexingDialog(
		size_t startedFileCount,
		size_t finishedFileCount,
		size_t totalFileCount,
		const std::vector<FilePath>& sourcePaths) override;
	DatabasePolicy finishedIndexingDialog(
		size_t indexedFileCount,
		size_t totalIndexedFileCount,
		size_t completedFileCount,
		size_t totalFileCount,
		float time,
		ErrorCountInfo errorInfo,
		bool interrupted,
		bool shallow) override;

	int confirm(const std::wstring& message, const std::vector<std::wstring>& options) override;

	void setParentWindow(QtWindow* window);

private slots:
	void showUnknownProgress(const std::wstring& title, const std::wstring& message, bool stacked);
	void hideUnknownProgress();

	void setUIBlocked(bool blocked);
	void dialogVisibilityChanged(bool visible);

private:
	void handleMessage(MessageErrorCountUpdate* message) override;
	void handleMessage(MessageIndexingShowDialog* message) override;
	void handleMessage(MessageWindowClosed* message) override;

	void updateErrorCount(size_t errorCount, size_t fatalCount);

	template <typename DialogType, typename... ParamTypes>
	DialogType* createWindow(ParamTypes... params);

	QtMainWindow* m_mainWindow;
	QtWindow* m_parentWindow;

	QtWindowStack m_windowStack;

	QtThreadedLambdaFunctor m_onQtThread;
	QtThreadedLambdaFunctor m_onQtThread2;
	QtThreadedLambdaFunctor m_onQtThread3;

	std::map<RefreshMode, RefreshInfo> m_refreshInfos;
	bool m_shallowIndexingEnabled;

	bool m_resultReady;
	bool m_uiBlocked = false;
	bool m_dialogsVisible = true;
};

#endif	  // QT_DIALOG_VIEW_H
