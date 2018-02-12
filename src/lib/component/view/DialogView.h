#ifndef DIALOG_VIEW_H
#define DIALOG_VIEW_H

#include <string>
#include <vector>

#include "data/ErrorCountInfo.h"
#include "project/RefreshInfo.h"

class Project;
class StorageAccess;

class DialogView
{
public:
	DialogView(StorageAccess* storageAccess);
	virtual ~DialogView();

	virtual void showUnknownProgressDialog(const std::wstring& title, const std::wstring& message);
	virtual void hideUnknownProgressDialog();

	virtual void showProgressDialog(const std::wstring& title, const std::wstring& message, size_t progress);
	virtual void hideProgressDialog();

	virtual void startIndexingDialog(
		Project* project, const std::vector<RefreshMode>& enabledModes, const RefreshInfo& info);
	virtual void updateIndexingDialog(
		size_t startedFileCount, size_t finishedFileCount, size_t totalFileCount, const FilePath& sourcePath);
	virtual void finishedIndexingDialog(
		size_t indexedFileCount, size_t totalIndexedFileCount, size_t completedFileCount, size_t totalFileCount,
		float time, ErrorCountInfo errorInfo, bool interrupted);

	virtual void hideDialogs(bool unblockUI = true);

	int confirm(const std::string& message);
	virtual int confirm(const std::string& message, const std::vector<std::string>& options);
	int confirm(const std::wstring& message);
	virtual int confirm(const std::wstring& message, const std::vector<std::wstring>& options);

protected:
	StorageAccess* m_storageAccess;
};

#endif // DIALOG_VIEW_H
