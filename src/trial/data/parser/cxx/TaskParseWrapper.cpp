#include "data/parser/cxx/TaskParseWrapper.h"

#include "data/PersistentStorage.h"
#include "utility/messaging/type/MessageFinishedParsing.h"

TaskParseWrapper::TaskParseWrapper(
	PersistentStorage* storage,
	std::shared_ptr<FileRegister> fileRegister,
	DialogView* dialogView
)
	: m_storage(storage)
{
}

TaskParseWrapper::~TaskParseWrapper()
{
}

void TaskParseWrapper::enter()
{
	m_storage->startParsing();

	m_task->enter();
}

Task::TaskState TaskParseWrapper::update()
{
	return m_task->update();
}

void TaskParseWrapper::exit()
{
	m_task->exit();

	m_storage->finishParsing();

	MessageFinishedParsing().dispatch();
}

void TaskParseWrapper::interrupt()
{
	m_task->interrupt();
}

void TaskParseWrapper::revert()
{
	m_task->revert();
}

void TaskParseWrapper::abort()
{
	m_task->abort();

	MessageFinishedParsing().dispatch();
}
