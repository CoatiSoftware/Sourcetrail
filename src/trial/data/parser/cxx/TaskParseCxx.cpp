#include "data/parser/cxx/TaskParseCxx.h"

#include "data/Storage.h"
#include "utility/messaging/type/MessageFinishedParsing.h"

TaskParseCxx::TaskParseCxx(
	Storage* storage,
	const FileManager* fileManager,
	const Parser::Arguments& arguments,
	const std::vector<FilePath>& files
)
	: m_storage(storage)
	, m_arguments(arguments)
{
}

std::vector<FilePath> TaskParseCxx::getSourceFilesFromCDB(const FilePath& compilationDatabasePath)
{
	return std::vector<FilePath>();
}

void TaskParseCxx::enter()
{
	m_storage->startParsing();
}

Task::TaskState TaskParseCxx::update()
{
	return Task::STATE_FINISHED;
}

void TaskParseCxx::exit()
{
	m_storage->finishParsing();

	MessageFinishedParsing(0, 0, 0).dispatch();
}

void TaskParseCxx::interrupt()
{
}

void TaskParseCxx::revert()
{
}
