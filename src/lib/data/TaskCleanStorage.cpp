#include "data/TaskCleanStorage.h"

#include "component/view/DialogView.h"
#include "data/PersistentStorage.h"
#include "utility/scheduling/Blackboard.h"
#include "utility/utility.h"

TaskCleanStorage::TaskCleanStorage(
	PersistentStorage* storage, const std::vector<FilePath>& filePaths, DialogView* dialogView
)
	: m_storage(storage)
	, m_filePaths(filePaths)
	, m_dialogView(dialogView)
{
}

void TaskCleanStorage::doEnter(std::shared_ptr<Blackboard> blackboard)
{
	m_dialogView->showStatusDialog("Clearing Files", std::to_string(m_filePaths.size()) + " Files");

	m_start = utility::durationStart();

	if (!m_filePaths.empty())
	{
		m_storage->setMode(SqliteStorage::STORAGE_MODE_CLEAR);
	}
}

Task::TaskState TaskCleanStorage::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	DialogView* dialogView = m_dialogView;
	m_storage->clearFileElements(m_filePaths, [=](int progress)
		{
			if (dialogView != nullptr)
			{
				dialogView->showProgressDialog("Clearing", std::to_string(m_filePaths.size()) + " Files", progress);
			}
		}
	);

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
