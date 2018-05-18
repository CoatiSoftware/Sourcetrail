#include "data/indexer/IndexerCommandCxxCdb.h"

#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Tooling/JSONCompilationDatabase.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageStatus.h"

std::vector<FilePath> IndexerCommandCxxCdb::getSourceFilesFromCDB(const FilePath& compilationDatabasePath)
{
	std::string error;
	std::shared_ptr<clang::tooling::JSONCompilationDatabase> cdb = std::shared_ptr<clang::tooling::JSONCompilationDatabase>
		(clang::tooling::JSONCompilationDatabase::loadFromFile(utility::encodeToUtf8(compilationDatabasePath.wstr()), error, clang::tooling::JSONCommandLineSyntax::AutoDetect));

	if (!error.empty())
	{
		const std::wstring message = L"Loading Clang compilation database failed with error: \"" + utility::decodeFromUtf8(error) + L"\"";
		LOG_ERROR(message);
		MessageStatus(message, true).dispatch();
	}

	std::vector<FilePath> filePaths;
	if (cdb)
	{
		for (const clang::tooling::CompileCommand& command : cdb->getAllCompileCommands())
		{
			FilePath path = FilePath(utility::decodeFromUtf8(command.Filename)).makeCanonical();
			if (!path.isAbsolute())
			{
				path = FilePath(utility::decodeFromUtf8(command.Directory + '/' + command.Filename)).makeCanonical();
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
	const std::set<FilePathFilter>& excludeFilters,
	const std::set<FilePathFilter>& includeFilters,
	const FilePath& workingDirectory,
	const std::vector<std::wstring>& compilerFlags,
	const std::vector<FilePath>& systemHeaderSearchPaths,
	const std::vector<FilePath>& frameworkSearchPaths
)
	: IndexerCommandCxx(
		sourceFilePath, 
		indexedPaths, 
		excludeFilters, 
		includeFilters, 
		workingDirectory, 
		systemHeaderSearchPaths,
		frameworkSearchPaths, 
		compilerFlags)
{
}

IndexerCommandType IndexerCommandCxxCdb::getIndexerCommandType() const
{
	return getStaticIndexerCommandType();
}

const FilePath& IndexerCommandCxxCdb::getWorkingDirectory() const
{
	return IndexerCommandCxx::getWorkingDirectory();
}
