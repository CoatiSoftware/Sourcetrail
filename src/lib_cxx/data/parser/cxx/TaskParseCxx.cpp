#include "data/parser/cxx/TaskParseCxx.h"

#include "clang/Tooling/JSONCompilationDatabase.h"

#include "component/view/DialogView.h"
#include "data/parser/cxx/CxxParser.h"
#include "data/parser/ParserClientImpl.h"
#include "data/StorageProvider.h"
#include "utility/file/FileRegister.h"
#include "utility/scheduling/Blackboard.h"

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
	std::shared_ptr<StorageProvider> storageProvider,
	std::shared_ptr<FileRegister> fileRegister,
	const Parser::Arguments& arguments,
	DialogView* dialogView
)
	: TaskParse(storageProvider, fileRegister, arguments, dialogView)
	, m_isCDB(false)
{
	if (arguments.compilationDatabasePath.exists())
	{
		m_isCDB = true;
	}
	m_parserClient = std::make_shared<ParserClientImpl>(); // todo: create one parserclient per file
	m_parser = std::make_shared<CxxParser>(m_parserClient.get(), fileRegister);
}

void TaskParseCxx::doEnter(std::shared_ptr<Blackboard> blackboard)
{
	TaskParse::doEnter(blackboard);

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

Task::TaskState TaskParseCxx::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	FileRegister* fileRegister = m_parser->getFileRegister();

	FilePath sourcePath = fileRegister->consumeSourceFile();

	if (sourcePath.empty())
	{
		return STATE_FAILURE;
	}
	else
	{
		m_dialogView->updateIndexingDialog(
			fileRegister->getParsedSourceFilesCount(), fileRegister->getSourceFilesCount(), sourcePath.str()
		);

		std::shared_ptr<IntermediateStorage> storage = m_storageProvider->popIndexerTarget();
		m_parserClient->setStorage(storage);
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
			m_storageProvider->pushIndexerTarget(storage);
		}
	}

	return (m_interrupted ? STATE_FAILURE : STATE_SUCCESS);
}
