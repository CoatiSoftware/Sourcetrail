#include "data/parser/cxx/TaskParseWrapper.h"

#include "data/PersistentStorage.h"
#include "utility/file/FileRegister.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/utility.h"

TaskParseWrapper::TaskParseWrapper(
	std::shared_ptr<Task> child,
	PersistentStorage* storage,
	std::shared_ptr<FileRegister> fileRegister
)
	: m_child(child)
	, m_storage(storage)
	, m_fileRegister(fileRegister)
{
}

void TaskParseWrapper::enter()
{
	m_start = utility::durationStart();
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

	MessageStatus("building search index", false, true).dispatch();

	m_storage->finishParsing();

	MessageFinishedParsing(
		m_fileRegister->getParsedSourceFilesCount(),
		m_fileRegister->getSourceFilesCount(),
		utility::duration(m_start)
	).dispatch();
}

void TaskParseWrapper::interrupt()
{
	MessageStatus("analyzing files interrupted", false, true).dispatch();
	m_child->interrupt();
}

void TaskParseWrapper::revert()
{
	m_child->revert();
}
