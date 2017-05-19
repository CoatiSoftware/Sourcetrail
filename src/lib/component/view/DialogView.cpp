#include "component/view/DialogView.h"

DialogView::DialogView(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
{
}

DialogView::~DialogView()
{
}

void DialogView::showUnknownProgressDialog(const std::string& title, const std::string& message)
{
}

void DialogView::hideUnknownProgressDialog()
{
}

void DialogView::showProgressDialog(const std::string& title, const std::string& message, int progress)
{
}

void DialogView::hideProgressDialog()
{
}

DialogView::IndexingOptions DialogView::startIndexingDialog(
	size_t cleanFileCount, size_t indexFileCount, size_t totalFileCount, DialogView::IndexingOptions options)
{
	return IndexingOptions();
}

void DialogView::updateIndexingDialog(
	size_t startedFileCount, size_t finishedFileCount, size_t totalFileCount, std::string sourcePath)
{
}

void DialogView::finishedIndexingDialog(
	size_t indexedFileCount, size_t totalIndexedFileCount, size_t completedFileCount, size_t totalFileCount,
	float time, ErrorCountInfo errorInfo, bool interrupted)
{
}

void DialogView::hideDialogs()
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
