#include "data/parser/cxx/TaskParseWrapper.h"

#include "data/PersistentStorage.h"
#include "utility/file/FileRegister.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/utility.h"

TaskParseWrapper::TaskParseWrapper(
	PersistentStorage* storage,
	std::shared_ptr<FileRegister> fileRegister
)
	: m_storage(storage)
	, m_fileRegister(fileRegister)
{
}

TaskParseWrapper::~TaskParseWrapper()
{
}

void TaskParseWrapper::enter()
{
	m_start = utility::durationStart();
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

	MessageStatus("optimizing database", false, true).dispatch();

	m_storage->optimizeMemory();

	MessageStatus("building caches", false, true).dispatch();

	m_storage->finishParsing();

	MessageFinishedParsing(
		m_fileRegister->getParsedSourceFilesCount(),
		m_fileRegister->getSourceFilesCount(),
		utility::duration(m_start)
	).dispatch();
}

void TaskParseWrapper::interrupt()
{
	MessageStatus("indexing files interrupted", false, true).dispatch();
	m_task->interrupt();
}

void TaskParseWrapper::revert()
{
	m_task->revert();
}
