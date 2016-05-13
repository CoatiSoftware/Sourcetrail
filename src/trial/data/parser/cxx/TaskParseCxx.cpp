#include "data/parser/cxx/TaskParseCxx.h"

TaskParseCxx::TaskParseCxx(
	PersistentStorage* storage,
	std::shared_ptr<std::mutex> storageMutex,
	std::shared_ptr<FileRegister> fileRegister,
	const Parser::Arguments& arguments
)
{
}

std::vector<FilePath> TaskParseCxx::getSourceFilesFromCDB(const FilePath& compilationDatabasePath)
{
	return std::vector<FilePath>();
}

void TaskParseCxx::enter()
{
}

Task::TaskState TaskParseCxx::update()
{
	return Task::STATE_FINISHED;
}

void TaskParseCxx::exit()
{
}

void TaskParseCxx::interrupt()
{
}

void TaskParseCxx::revert()
{
}
