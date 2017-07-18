#include "data/indexer/IndexerCommandCxxCdb.h"

#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Tooling/JSONCompilationDatabase.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageStatus.h"

std::vector<FilePath> IndexerCommandCxxCdb::getSourceFilesFromCDB(const FilePath& compilationDatabasePath)
{
	std::string error;
	std::shared_ptr<clang::tooling::JSONCompilationDatabase> cdb = std::shared_ptr<clang::tooling::JSONCompilationDatabase>
		(clang::tooling::JSONCompilationDatabase::loadFromFile(compilationDatabasePath.str(), error, clang::tooling::JSONCommandLineSyntax::AutoDetect));

	if (!error.empty())
	{
		const std::string message = "Loading Clang compilation database failed with error: \"" + error + "\"";
		LOG_ERROR(message);
		MessageStatus(message, true).dispatch();
	}

	std::vector<FilePath> filePaths;
	if (cdb)
	{
		for (const clang::tooling::CompileCommand& command : cdb->getAllCompileCommands())
		{
			FilePath path = FilePath(command.Filename).canonical();
			if (!path.isAbsolute())
			{
				path = FilePath(command.Directory + '/' + command.Filename).canonical();
			}
			filePaths.push_back(path);
		}
	}
	return filePaths;
}

IndexerCommandType IndexerCommandCxxCdb::getStaticIndexerCommandType()
{
	return INDEXER_COMMAND_CXX_CDB;
}

IndexerCommandCxxCdb::IndexerCommandCxxCdb(
	const FilePath& sourceFilePath,
	const std::set<FilePath>& indexedPaths,
	const std::set<FilePath>& excludedPaths,
	const FilePath& workingDirectory,
	const std::vector<std::string>& compilerFlags,
	const std::vector<FilePath>& systemHeaderSearchPaths,
	const std::vector<FilePath>& frameworkSearchPaths
)
	: IndexerCommandCxx(sourceFilePath, indexedPaths, excludedPaths, systemHeaderSearchPaths, frameworkSearchPaths, compilerFlags)
	, m_workingDirectory(workingDirectory)
{
}

IndexerCommandCxxCdb::~IndexerCommandCxxCdb()
{
}

IndexerCommandType IndexerCommandCxxCdb::getIndexerCommandType() const
{
	return getStaticIndexerCommandType();
}

size_t IndexerCommandCxxCdb::getByteSize() const
{
	return IndexerCommandCxx::getByteSize() + sizeof(std::string) + m_workingDirectory.str().size();
}

FilePath IndexerCommandCxxCdb::getWorkingDirectory() const
{
	return m_workingDirectory;
}
