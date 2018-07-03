#include "component/view/DialogView.h"

DialogView::DialogView(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
{
}

DialogView::~DialogView()
{
}

void DialogView::showUnknownProgressDialog(const std::wstring& title, const std::wstring& message)
{
}

void DialogView::hideUnknownProgressDialog()
{
}

void DialogView::showProgressDialog(const std::wstring& title, const std::wstring& message, size_t progress)
{
}

void DialogView::hideProgressDialog()
{
}

void DialogView::startIndexingDialog(
	Project* project, const std::vector<RefreshMode>& enabledModes, const RefreshInfo& info)
{
}

void DialogView::updateIndexingDialog(
	size_t startedFileCount, size_t finishedFileCount, size_t totalFileCount, const FilePath& sourcePath)
{
}

DatabasePolicy DialogView::finishedIndexingDialog(
	size_t indexedFileCount, size_t totalIndexedFileCount, size_t completedFileCount, size_t totalFileCount,
	float time, ErrorCountInfo errorInfo, bool interrupted)
{
	return DATABASE_POLICY_KEEP; // used in non-gui mode
}

void DialogView::hideDialogs(bool unblockUI)
{
}

int DialogView::confirm(const std::string& message)
{
	return confirm(message, std::vector<std::string>());
}

int DialogView::confirm(const std::string& message, const std::vector<std::string>& options)
{
	return -1;
}

int DialogView::confirm(const std::wstring& message)
{
	return confirm(message, std::vector<std::wstring>());
}

int DialogView::confirm(const std::wstring& message, const std::vector<std::wstring>& options)
{
	return -1;
}
