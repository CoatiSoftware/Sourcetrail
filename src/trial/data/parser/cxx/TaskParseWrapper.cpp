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

void TaskParseWrapper::setTask(std::shared_ptr<Task> task)
{
}

void TaskParseWrapper::doEnter()
{
}

Task::TaskState TaskParseWrapper::doUpdate()
{
	return STATE_SUCCESS;
}

void TaskParseWrapper::doExit()
{
}

void TaskParseWrapper::doReset()
{
}
