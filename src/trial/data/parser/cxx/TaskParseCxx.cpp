#include "data/parser/cxx/TaskParseCxx.h"

TaskParseCxx::TaskParseCxx(
	std::shared_ptr<StorageProvider> storageProvider,
	std::shared_ptr<FileRegister> fileRegister,
	const Parser::Arguments& arguments,
	DialogView* dialogView
)
{
}

std::vector<FilePath> TaskParseCxx::getSourceFilesFromCDB(const FilePath& compilationDatabasePath)
{
	return std::vector<FilePath>();
}

void TaskParseCxx::doEnter(std::shared_ptr<Blackboard> blackboard)
{
}

Task::TaskState TaskParseCxx::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	return STATE_SUCCESS;
}

void TaskParseCxx::doExit(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskParseCxx::doReset(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskParseCxx::handleMessage(MessageInterruptTasks* message)
{
}
