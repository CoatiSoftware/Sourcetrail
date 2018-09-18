#include "TaskCleanStorage.h"

#include "DialogView.h"
#include "PersistentStorage.h"
#include "FilePath.h"
#include "Blackboard.h"
#include "utility.h"
#include "Application.h"

TaskCleanStorage::TaskCleanStorage(
	std::weak_ptr<PersistentStorage> storage, std::shared_ptr<DialogView> dialogView,
	const std::vector<FilePath>& filePaths, bool clearAllErrors
)
	: m_storage(storage)
	, m_dialogView(dialogView)
	, m_filePaths(filePaths)
	, m_clearAllErrors(clearAllErrors)
{
}

void TaskCleanStorage::doEnter(std::shared_ptr<Blackboard> blackboard)
{
	m_dialogView->showUnknownProgressDialog(L"Clearing Files", std::to_wstring(m_filePaths.size()) + L" Files");

	m_start = utility::durationStart();

	if (!m_filePaths.empty() || m_clearAllErrors)
	{
		if (std::shared_ptr<PersistentStorage> storage = m_storage.lock())
		{
			storage->setMode(SqliteIndexStorage::STORAGE_MODE_CLEAR);
		}
	}
}

Task::TaskState TaskCleanStorage::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	if (std::shared_ptr<PersistentStorage> storage = m_storage.lock())
	{
		if (m_clearAllErrors)
		{
			storage->clearAllErrors();
		}

		storage->clearFileElements(m_filePaths, [=](int progress)
			{
				m_dialogView->showProgressDialog(L"Clearing", std::to_wstring(m_filePaths.size()) + L" Files", progress);
			}
		);
	}

	m_filePaths.clear();

	return STATE_SUCCESS;
}

void TaskCleanStorage::doExit(std::shared_ptr<Blackboard> blackboard)
{
	blackboard->set("clear_time", utility::duration(m_start));

	m_dialogView->hideProgressDialog();
}

void TaskCleanStorage::doReset(std::shared_ptr<Blackboard> blackboard)
{
}
