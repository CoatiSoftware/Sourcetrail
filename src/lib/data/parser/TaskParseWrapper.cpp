#include "data/parser/TaskParseWrapper.h"

#include "component/view/DialogView.h"
#include "data/PersistentStorage.h"
#include "utility/scheduling/Blackboard.h"
#include "utility/utility.h"

TaskParseWrapper::TaskParseWrapper(
	PersistentStorage* storage,
	DialogView* dialogView
)
	: m_storage(storage)
	, m_dialogView(dialogView)
{
}

TaskParseWrapper::~TaskParseWrapper()
{
}

void TaskParseWrapper::setTask(std::shared_ptr<Task> task)
{
	if (task)
	{
		m_taskRunner = std::make_shared<TaskRunner>(task);
	}
}

void TaskParseWrapper::doEnter(std::shared_ptr<Blackboard> blackboard)
{
	int sourceFileCount = 0;
	blackboard->get("source_file_count", sourceFileCount);
	m_dialogView->updateIndexingDialog(0, sourceFileCount, "");

	m_start = utility::durationStart();

	if (sourceFileCount > 0)
	{
		m_storage->setMode(SqliteStorage::STORAGE_MODE_WRITE);
	}
}

Task::TaskState TaskParseWrapper::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	return m_taskRunner->update(blackboard);
}

void TaskParseWrapper::doExit(std::shared_ptr<Blackboard> blackboard)
{
	blackboard->set("index_time", utility::duration(m_start));
}

void TaskParseWrapper::doReset(std::shared_ptr<Blackboard> blackboard)
{
	m_taskRunner->reset();
}
