#include "DialogView.h"

DialogView::DialogView(UseCase useCase, StorageAccess* storageAccess)
	: m_useCase(useCase)
	, m_storageAccess(storageAccess)
{
}

DialogView::UseCase DialogView::getUseCase() const
{
	return m_useCase;
}

void DialogView::setDialogsHideable(bool hideable)
{
	m_dialogsHideable = hideable;
}

bool DialogView::dialogsHidden() const
{
	return false;
}

void DialogView::clearDialogs()
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
	Project* project, const std::vector<RefreshMode>& enabledModes, const RefreshMode initialMode,
	std::function<void(const RefreshInfo& info)> onStartIndexing, std::function<void()> onCancelIndexing)
{
}

void DialogView::updateIndexingDialog(
	size_t startedFileCount, size_t finishedFileCount, size_t totalFileCount, const std::vector<FilePath>& sourcePaths)
{
}

void DialogView::updateCustomIndexingDialog(
	size_t startedFileCount, size_t finishedFileCount, size_t totalFileCount, const std::vector<FilePath>& sourcePaths)
{
}

DatabasePolicy DialogView::finishedIndexingDialog(
	size_t indexedFileCount, size_t totalIndexedFileCount, size_t completedFileCount, size_t totalFileCount,
	float time, ErrorCountInfo errorInfo, bool interrupted)
{
	return DATABASE_POLICY_KEEP; // used in non-gui mode
}

int DialogView::confirm(const std::wstring& message)
{
	return confirm(message, std::vector<std::wstring>());
}

int DialogView::confirm(const std::wstring& message, const std::vector<std::wstring>& options)
{
	return -1;
}
