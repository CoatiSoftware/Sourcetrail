#include "data/parser/cxx/TaskParseCxx.h"

#include "data/parser/ParserClient.h"
#include "utility/messaging/type/MessageFinishedParsing.h"

TaskParseCxx::TaskParseCxx(
	ParserClient* client,
	const FileManager* fileManager,
	const Parser::Arguments& arguments,
	const std::vector<FilePath>& files
)
	: m_client(client)
	, m_arguments(arguments)
{
}

std::vector<FilePath> TaskParseCxx::getSourceFilesFromCDB(const FilePath& compilationDatabasePath)
{
	return std::vector<FilePath>();
}

void TaskParseCxx::enter()
{
	m_client->startParsing();
}

Task::TaskState TaskParseCxx::update()
{
	return Task::STATE_FINISHED;
}

void TaskParseCxx::exit()
{
	m_client->finishParsing();

	MessageFinishedParsing(0, 0, 0).dispatch();
}

void TaskParseCxx::interrupt()
{
}

void TaskParseCxx::revert()
{
}
