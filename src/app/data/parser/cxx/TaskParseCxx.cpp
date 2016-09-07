#include "data/parser/cxx/TaskParseCxx.h"

#include <sstream>

#include "clang/Tooling/JSONCompilationDatabase.h"

#include "component/view/DialogView.h"
#include "data/parser/cxx/CxxParser.h"
#include "data/IntermediateStorage.h"
#include "utility/file/FileRegister.h"
#include "utility/utility.h"

std::vector<FilePath> TaskParseCxx::getSourceFilesFromCDB(const FilePath& compilationDatabasePath)
{
	std::string error;
	std::shared_ptr<clang::tooling::JSONCompilationDatabase> cdb = std::shared_ptr<clang::tooling::JSONCompilationDatabase>
		(clang::tooling::JSONCompilationDatabase::loadFromFile(compilationDatabasePath.str(), error));

	std::vector<FilePath> filePaths;
	if (cdb)
	{
		std::vector<std::string> files = cdb->getAllFiles();
		for (const std::string& file : files)
		{
			filePaths.push_back(FilePath(file));
		}
	}
	return filePaths;
}

TaskParseCxx::TaskParseCxx(
	std::shared_ptr<IntermediateStorage> storage,
	std::shared_ptr<FileRegister> fileRegister,
	const Parser::Arguments& arguments,
	DialogView* dialogView
)
	: m_storage(storage)
	, m_arguments(arguments)
	, m_dialogView(dialogView)
	, m_isCDB(false)
	, m_interrupted(false)
{
	if (arguments.compilationDatabasePath.exists())
	{
		m_isCDB = true;
	}
	m_parserClient = std::make_shared<ParserClientImpl>(); // todo: create one parserclient per file
	m_parser = std::make_shared<CxxParser>(m_parserClient.get(), fileRegister);
}

void TaskParseCxx::doEnter()
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

Task::TaskState TaskParseCxx::doUpdate()
{
	FileRegister* fileRegister = m_parser->getFileRegister();

	FilePath sourcePath = fileRegister->consumeSourceFile();

	if (!sourcePath.empty())
	{
		m_dialogView->updateIndexingDialog(
			fileRegister->getParsedSourceFilesCount(), fileRegister->getSourceFilesCount(), sourcePath.str()
		);

		m_storage->clear();
		m_parserClient->setStorage(m_storage);
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

		if (!m_interrupted)
		{
			fileRegister->markThreadFilesParsed();
		}
	}

	return (m_interrupted ? STATE_FAILURE : STATE_SUCCESS);
}

void TaskParseCxx::doExit()
{
}

void TaskParseCxx::doReset()
{
}

void TaskParseCxx::handleMessage(MessageInterruptTasks* message)
{
	m_interrupted = true;
}
