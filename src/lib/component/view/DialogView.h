#ifndef DIALOG_VIEW_H
#define DIALOG_VIEW_H

#include <string>
#include <vector>

#include "data/ErrorCountInfo.h"

class StorageAccess;

class DialogView
{
public:
	enum IndexMode
	{
		INDEX_ABORT,
		INDEX_REFRESH,
		INDEX_FULL
	};

	DialogView(StorageAccess* storageAccess);
	virtual ~DialogView();

	virtual void showProgressDialog(const std::string& title, const std::string& message);
	virtual void hideProgressDialog();

	virtual IndexMode startIndexingDialog(
		size_t cleanFileCount, size_t indexFileCount, size_t totalFileCount, bool forceRefresh, bool needsFullRefresh);
	virtual void updateIndexingDialog(size_t fileCount, size_t totalFileCount, std::string sourcePath);
	virtual void finishedIndexingDialog(size_t fileCount, size_t totalFileCount, float time, ErrorCountInfo errorInfo);

	int confirm(const std::string& message);
	virtual int confirm(const std::string& message, const std::vector<std::string>& options);

protected:
	StorageAccess* m_storageAccess;
};

#endif // DIALOG_VIEW_H
