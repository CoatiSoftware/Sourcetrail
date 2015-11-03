#include "data/parser/cxx/TaskParseCxx.h"

#include <sstream>

#include "data/parser/ParserClient.h"
#include "utility/file/FileRegister.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/utility.h"

TaskParseCxx::TaskParseCxx(
	ParserClient* client,
	const FileManager* fileManager,
	const Parser::Arguments& arguments,
	const std::vector<FilePath>& files
)
	: m_client(client)
	, m_parser(client, fileManager)
	, m_arguments(arguments)
	, m_files(files)
{
}

void TaskParseCxx::enter()
{
	m_start = utility::durationStart();

	m_parser.setupParsing(m_files, m_arguments);

	for (const FilePath& path : m_parser.getFileRegister()->getUnparsedSourceFilePaths())
	{
		m_sourcePaths.push(path.absolute());
	}

	m_client->startParsing();
}

Task::TaskState TaskParseCxx::update()
{
	FilePath sourcePath;
	bool isSource = false;

	FileRegister* fileRegister = m_parser.getFileRegister();

	if (m_sourcePaths.size())
	{
		sourcePath = m_sourcePaths.front();
		m_sourcePaths.pop();
		isSource = true;
	}
	else
	{
		std::vector<FilePath> unparsedHeaders = fileRegister->getUnparsedIncludeFilePaths();
		if (unparsedHeaders.size())
		{
			sourcePath = unparsedHeaders[0];
		}
	}

	if (sourcePath.empty())
	{
		return Task::STATE_FINISHED;
	}

	std::stringstream ss;
	ss << "analyzing files (ESC to quit): [";
	ss << fileRegister->getParsedFilesCount() << "/" << fileRegister->getFilesCount() << "] ";
	ss << sourcePath.str();

	MessageStatus(ss.str(), false, true).dispatch();

	m_client->prepareParsingFile(sourcePath);

	m_parser.runTool(std::vector<std::string>(1, sourcePath.str()));

	m_client->finishParsingFile(sourcePath);

	if (isSource)
	{
		fileRegister->markSourceFileParsed(sourcePath.str());
	}

	return Task::STATE_RUNNING;
}

void TaskParseCxx::exit()
{
	MessageStatus("building search index").dispatch();

	m_client->finishParsing();

	FileRegister* fileRegister = m_parser.getFileRegister();

	MessageFinishedParsing(
		fileRegister->getParsedFilesCount(),
		fileRegister->getFilesCount(),
		utility::duration(m_start),
		m_parser.getParserClient()->getErrorCount()
	).dispatch();
}

void TaskParseCxx::interrupt()
{
	MessageStatus("analyzing files interrupted", false, true).dispatch();
}

void TaskParseCxx::revert()
{
}
