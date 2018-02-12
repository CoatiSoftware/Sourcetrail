#include "data/TaskCleanStorage.h"

#include "component/view/DialogView.h"
#include "data/storage/PersistentStorage.h"
#include "utility/file/FilePath.h"
#include "utility/scheduling/Blackboard.h"
#include "utility/utility.h"
#include "Application.h"

TaskCleanStorage::TaskCleanStorage(
	PersistentStorage* storage, const std::vector<FilePath>& filePaths
)
	: m_storage(storage)
	, m_filePaths(filePaths)
{
}

void TaskCleanStorage::doEnter(std::shared_ptr<Blackboard> blackboard)
{
	Application::getInstance()->getDialogView()->showUnknownProgressDialog(
		L"Clearing Files", std::to_wstring(m_filePaths.size()) + L" Files");

	m_start = utility::durationStart();

	if (!m_filePaths.empty())
	{
		m_storage->setMode(SqliteIndexStorage::STORAGE_MODE_CLEAR);
	}
}

Task::TaskState TaskCleanStorage::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	m_storage->clearFileElements(m_filePaths, [=](int progress)
		{
			Application::getInstance()->getDialogView()->showProgressDialog(
				L"Clearing", std::to_wstring(m_filePaths.size()) + L" Files", progress);
		}
	);

	m_filePaths.clear();

	return STATE_SUCCESS;
}

void TaskCleanStorage::doExit(std::shared_ptr<Blackboard> blackboard)
{
	blackboard->set("clear_time", utility::duration(m_start));

	Application::getInstance()->getDialogView()->hideProgressDialog();
}

void TaskCleanStorage::doReset(std::shared_ptr<Blackboard> blackboard)
{
}
