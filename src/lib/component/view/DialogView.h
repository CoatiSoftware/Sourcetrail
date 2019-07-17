#ifndef DIALOG_VIEW_H
#define DIALOG_VIEW_H

#include <functional>
#include <string>
#include <vector>

#include "ErrorCountInfo.h"
#include "RefreshInfo.h"

class Project;
class StorageAccess;

enum DatabasePolicy
{
	DATABASE_POLICY_KEEP,
	DATABASE_POLICY_DISCARD,
	DATABASE_POLICY_UNKNOWN
};

class DialogView
{
public:
	enum class UseCase
	{
		GENERAL,
		INDEXING,
		PROJECT_SETUP
	};

	DialogView(UseCase useCase, StorageAccess* storageAccess);
	virtual ~DialogView() = default;

	UseCase getUseCase() const;

	void setDialogsHideable(bool hideable);

	virtual bool dialogsHidden() const;
	virtual void clearDialogs();

	virtual void showUnknownProgressDialog(const std::wstring& title, const std::wstring& message);
	virtual void hideUnknownProgressDialog();

	virtual void showProgressDialog(const std::wstring& title, const std::wstring& message, size_t progress);
	virtual void hideProgressDialog();

	virtual void startIndexingDialog(
		Project* project, const std::vector<RefreshMode>& enabledModes, const RefreshMode initialMode,
		std::function<void(const RefreshInfo& info)> onStartIndexing, std::function<void()> onCancelIndexing);
	virtual void updateIndexingDialog(
		size_t startedFileCount, size_t finishedFileCount, size_t totalFileCount, const std::vector<FilePath>& sourcePaths);
	virtual void updateCustomIndexingDialog(
		size_t startedFileCount, size_t finishedFileCount, size_t totalFileCount, const std::vector<FilePath>& sourcePaths);
	virtual DatabasePolicy finishedIndexingDialog(
		size_t indexedFileCount, size_t totalIndexedFileCount, size_t completedFileCount, size_t totalFileCount,
		float time, ErrorCountInfo errorInfo, bool interrupted);

	int confirm(const std::wstring& message);
	virtual int confirm(const std::wstring& message, const std::vector<std::wstring>& options);

protected:
	const UseCase m_useCase;
	StorageAccess* m_storageAccess;

	bool m_dialogsHideable = false;
};

#endif // DIALOG_VIEW_H
