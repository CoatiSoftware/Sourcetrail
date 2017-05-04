#include "data/indexer/IndexerCommandCxxCdb.h"

#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Tooling/JSONCompilationDatabase.h"

std::vector<FilePath> IndexerCommandCxxCdb::getSourceFilesFromCDB(const FilePath& compilationDatabasePath)
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

std::string IndexerCommandCxxCdb::getIndexerKindString()
{
	return "CxxCdb";
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

std::string IndexerCommandCxxCdb::getKindString() const
{
	return getIndexerKindString();
}

size_t IndexerCommandCxxCdb::getByteSize() const
{
	return IndexerCommandCxx::getByteSize() + sizeof(*this) + m_workingDirectory.str().size();
}

FilePath IndexerCommandCxxCdb::getWorkingDirectory() const
{
	return m_workingDirectory;
}
