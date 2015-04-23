#include "data/parser/cxx/TaskParseCxx.h"

#include <sstream>

#include "utility/file/FileRegister.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageStatus.h"

#include "data/parser/ParserClient.h"

TaskParseCxx::TaskParseCxx(
	ParserClient* client,
	const FileManager* fileManager,
	const Parser::Arguments& arguments,
	const std::vector<FilePath>& files
)
	: m_parser(client, fileManager)
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
		m_sourcePaths.push(path.absoluteStr());
	}
}

Task::TaskState TaskParseCxx::update()
{
	std::string sourcePath;
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
			sourcePath = unparsedHeaders[0].str();
		}
	}

	if (!sourcePath.size())
	{
		return Task::STATE_FINISHED;
	}

	std::stringstream ss;
	ss << "parsing (ESC to quit): [";
	ss << fileRegister->getParsedFilesCount() << "/" << fileRegister->getFilesCount() << "] ";
	ss << sourcePath;

	MessageStatus(ss.str()).dispatch();

	m_parser.runTool(std::vector<std::string>(1, sourcePath));

	if (isSource)
	{
		fileRegister->markSourceFileParsed(sourcePath);
	}

	return Task::STATE_RUNNING;
}

void TaskParseCxx::exit()
{
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
}

void TaskParseCxx::revert()
{
}
