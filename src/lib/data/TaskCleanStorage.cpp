#include "data/TaskCleanStorage.h"

#include "component/view/DialogView.h"
#include "data/PersistentStorage.h"

TaskCleanStorage::TaskCleanStorage(
	PersistentStorage* storage, const std::vector<FilePath>& filePaths, DialogView* dialogView
)
	: m_storage(storage)
	, m_filePaths(filePaths)
	, m_dialogView(dialogView)
{
}

void TaskCleanStorage::enter()
{
	m_dialogView->showProgressDialog("Clearing Files", std::to_string(m_filePaths.size()) + " Files");
}

Task::TaskState TaskCleanStorage::update()
{
	m_storage->clearFileElements(m_filePaths);

	m_filePaths.clear();

	return Task::STATE_FINISHED;
}

void TaskCleanStorage::exit()
{
	m_dialogView->hideProgressDialog();
}

void TaskCleanStorage::interrupt()
{
}

void TaskCleanStorage::revert()
{
}

void TaskCleanStorage::abort()
{
}
