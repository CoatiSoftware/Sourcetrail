#include "data/parser/cxx/TaskParseCxx.h"

TaskParseCxx::TaskParseCxx(
	std::shared_ptr<IntermediateStorage> storage,
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

void TaskParseCxx::doEnter()
{
}

Task::TaskState TaskParseCxx::doUpdate()
{
	return STATE_SUCCESS;
}

void TaskParseCxx::doExit()
{
}

void TaskParseCxx::doReset()
{
}

void TaskParseCxx::handleMessage(MessageInterruptTasks* message)
{
}
