#include "data/parser/cxx/TaskParseCxx.h"

TaskParseCxx::TaskParseCxx(
	ParserClient* client,
	const FileManager* fileManager,
	const Parser::Arguments& arguments,
	const std::vector<FilePath>& files
)
	: m_arguments(arguments)
{
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
