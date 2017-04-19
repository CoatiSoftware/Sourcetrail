#include "data/TaskCleanStorage.h"

#include "component/view/DialogView.h"
#include "data/PersistentStorage.h"
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
		"Clearing Files", std::to_string(m_filePaths.size()) + " Files");

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
				"Clearing", std::to_string(m_filePaths.size()) + " Files", progress);
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
