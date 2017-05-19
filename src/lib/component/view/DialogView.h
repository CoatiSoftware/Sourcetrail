#ifndef DIALOG_VIEW_H
#define DIALOG_VIEW_H

#include <string>
#include <vector>

#include "data/ErrorCountInfo.h"

class StorageAccess;

class DialogView
{
public:
	struct IndexingOptions
	{
		IndexingOptions()
			: startIndexing(false)
			, fullRefreshVisible(false)
			, fullRefresh(false)
			, preprocessorOnlyVisible(false)
			, preprocessorOnly(false)
		{}

		bool startIndexing;

		bool fullRefreshVisible;
		bool fullRefresh;

		bool preprocessorOnlyVisible;
		bool preprocessorOnly;
	};

	DialogView(StorageAccess* storageAccess);
	virtual ~DialogView();

	virtual void showUnknownProgressDialog(const std::string& title, const std::string& message);
	virtual void hideUnknownProgressDialog();

	virtual void showProgressDialog(const std::string& title, const std::string& message, int progress);
	virtual void hideProgressDialog();

	virtual IndexingOptions startIndexingDialog(
		size_t cleanFileCount, size_t indexFileCount, size_t totalFileCount, IndexingOptions options);
	virtual void updateIndexingDialog(
		size_t startedFileCount, size_t finishedFileCount, size_t totalFileCount, std::string sourcePath);
	virtual void finishedIndexingDialog(
		size_t indexedFileCount, size_t totalIndexedFileCount, size_t completedFileCount, size_t totalFileCount,
		float time, ErrorCountInfo errorInfo, bool interrupted);

	virtual void hideDialogs();

	int confirm(const std::string& message);
	virtual int confirm(const std::string& message, const std::vector<std::string>& options);

protected:
	StorageAccess* m_storageAccess;
};

#endif // DIALOG_VIEW_H
