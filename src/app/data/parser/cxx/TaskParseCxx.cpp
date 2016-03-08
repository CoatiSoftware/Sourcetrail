#include "data/parser/cxx/TaskParseCxx.h"

#include <sstream>

#include "clang/Tooling/JSONCompilationDatabase.h"

#include "data/parser/cxx/CxxParser.h"
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
	, m_parser(std::make_shared<CxxParser>(client, fileManager))
	, m_arguments(arguments)
	, m_files(files)
	, m_isCDB(false)
{
	if (arguments.compilationDatabasePath.exists())
	{
		m_isCDB = true;
	}
}

std::vector<FilePath> TaskParseCxx::getSourceFilesFromCDB(const FilePath& compilationDatabasePath)
{
	std::string error;
	std::shared_ptr<clang::tooling::JSONCompilationDatabase> cdb = std::shared_ptr<clang::tooling::JSONCompilationDatabase>
		(clang::tooling::JSONCompilationDatabase::loadFromFile(compilationDatabasePath.str(), error));

	std::vector<std::string> files = cdb->getAllFiles();
	std::vector<FilePath> filePaths;
	for (const std::string& file : files)
	{
		filePaths.push_back(FilePath(file));
	}
	return filePaths;
}

void TaskParseCxx::enter()
{
	m_start = utility::durationStart();

	if (m_isCDB)
	{
		std::string error;
		m_cdb = std::shared_ptr<clang::tooling::JSONCompilationDatabase>
			(clang::tooling::JSONCompilationDatabase::loadFromFile(m_arguments.compilationDatabasePath.str(), error));

		m_parser->setupParsingCDB(m_files, m_arguments);
	}
	else
	{
		m_parser->setupParsing(m_files, m_arguments);
	}

	for (const FilePath& path : m_parser->getFileRegister()->getUnparsedSourceFilePaths())
	{
		m_sourcePaths.push_back(path.absolute());
	}

	m_client->startParsing();
}

Task::TaskState TaskParseCxx::update()
{
	FilePath sourcePath;
	bool isSource = false;

	FileRegister* fileRegister = m_parser->getFileRegister();

	if (m_sourcePaths.size())
	{
		sourcePath = m_sourcePaths.front();
		m_sourcePaths.pop_front();
		isSource = true;
	}
	else if (!m_isCDB)
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
	ss << (m_isCDB ? fileRegister->getParsedSourceFilesCount() : fileRegister->getParsedFilesCount()) + 1 << "/";
	ss << (m_isCDB ? fileRegister->getSourceFilesCount() : fileRegister->getFilesCount()) << "] ";
	ss << sourcePath.str();

	MessageStatus(ss.str(), false, true).dispatch();

	m_client->startParsingFile(sourcePath);

	if (m_isCDB)
	{
		std::vector<clang::tooling::CompileCommand> commands = m_cdb->getCompileCommands(sourcePath.str());
		if (commands.size() > 0)
		{
			m_parser->runTool(commands[0], m_arguments);
		}
	}
	else
	{
		m_parser->runTool(std::vector<std::string>(1, sourcePath.str()));
	}

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

	FileRegister* fileRegister = m_parser->getFileRegister();

	MessageFinishedParsing(
		(m_isCDB ? fileRegister->getParsedSourceFilesCount() : fileRegister->getParsedFilesCount()),
		(m_isCDB ? fileRegister->getSourceFilesCount() : fileRegister->getFilesCount()),
		utility::duration(m_start)
	).dispatch();
}

void TaskParseCxx::interrupt()
{
	MessageStatus("analyzing files interrupted", false, true).dispatch();
}

void TaskParseCxx::revert()
{
}
