#include "data/parser/cxx/TaskParseWrapper.h"

#include "data/PersistentStorage.h"
#include "utility/messaging/type/MessageFinishedParsing.h"

TaskParseWrapper::TaskParseWrapper(
	std::shared_ptr<Task> child,
	PersistentStorage* storage,
	std::shared_ptr<FileRegister> fileRegister
)
	: m_child(child)
	, m_storage(storage)
{
}

void TaskParseWrapper::enter()
{
	m_storage->startParsing();

	m_child->enter();
}

Task::TaskState TaskParseWrapper::update()
{
	return m_child->update();
}

void TaskParseWrapper::exit()
{
	m_child->exit();

	m_storage->finishParsing();

	MessageFinishedParsing(0, 0, 0).dispatch();
}

void TaskParseWrapper::interrupt()
{
	m_child->interrupt();
}

void TaskParseWrapper::revert()
{
	m_child->revert();
}
