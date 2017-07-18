#include "data/parser/TaskParseWrapper.h"

#include "component/view/DialogView.h"
#include "data/storage/PersistentStorage.h"
#include "utility/scheduling/Blackboard.h"
#include "utility/utility.h"
#include "Application.h"

TaskParseWrapper::TaskParseWrapper(PersistentStorage* storage)
	: m_storage(storage)
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
	if (std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView())
	{
		dialogView->updateIndexingDialog(0, 0, sourceFileCount, "");
    }

	m_start = utility::durationStart();

	if (sourceFileCount > 0)
	{
		m_storage->setMode(SqliteIndexStorage::STORAGE_MODE_WRITE);
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

void TaskParseWrapper::doTerminate()
{
	m_taskRunner->terminate();
}
