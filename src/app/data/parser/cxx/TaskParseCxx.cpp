#include "data/parser/cxx/TaskParseCxx.h"

#include <sstream>

#include "clang/Tooling/JSONCompilationDatabase.h"

#include "data/parser/cxx/CxxParser.h"
#include "data/PersistentStorage.h"
#include "utility/file/FileRegister.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/utility.h"

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

TaskParseCxx::TaskParseCxx(
	PersistentStorage* storage,
	std::shared_ptr<std::mutex> storageMutex,
	std::shared_ptr<FileRegister> fileRegister,
	const Parser::Arguments& arguments
)
	: m_storage(storage)
	, m_storageMutex(storageMutex)
	, m_arguments(arguments)
	, m_isCDB(false)
{
	if (arguments.compilationDatabasePath.exists())
	{
		m_isCDB = true;
	}
	m_parserClient = std::make_shared<ParserClientImpl>(); // todo: create one parserclient per file
	m_parser = std::make_shared<CxxParser>(m_parserClient.get(), fileRegister);
}

void TaskParseCxx::enter()
{
	if (m_isCDB)
	{
		std::string error;
		m_cdb = std::shared_ptr<clang::tooling::JSONCompilationDatabase>
			(clang::tooling::JSONCompilationDatabase::loadFromFile(m_arguments.compilationDatabasePath.str(), error));

		m_parser->setupParsingCDB(m_arguments);
	}
	else
	{
		m_parser->setupParsing(m_arguments);
	}
}

Task::TaskState TaskParseCxx::update()
{
	FileRegister* fileRegister = m_parser->getFileRegister();

	FilePath sourcePath = fileRegister->consumeSourceFile();

	if (sourcePath.empty())
	{
		return Task::STATE_FINISHED;
	}

	std::stringstream ss;
	ss << "indexing files (ESC to quit): [";
	ss << fileRegister->getParsedSourceFilesCount() << "/";
	ss << fileRegister->getSourceFilesCount() << "] ";
	ss << sourcePath.str();
	MessageStatus(ss.str(), false, true).dispatch();

	std::shared_ptr<IntermediateStorage> intermediateStorage = std::make_shared<IntermediateStorage>();

	m_parserClient->setStorage(intermediateStorage);
	m_parserClient->startParsingFile();

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

	m_parserClient->finishParsingFile();
	m_parserClient->resetStorage();

	{
		std::lock_guard<std::mutex> lock(*(m_storageMutex.get()));
		m_storage->inject(intermediateStorage.get());
	}

	fileRegister->markThreadFilesParsed();

	return Task::STATE_RUNNING;
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
