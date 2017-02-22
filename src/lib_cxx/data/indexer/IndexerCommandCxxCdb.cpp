#include "data/indexer/IndexerCommandCxxCdb.h"

#include "clang/Tooling/CompilationDatabase.h"

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
	: IndexerCommand(sourceFilePath, indexedPaths, excludedPaths)
	, m_workingDirectory(workingDirectory)
	, m_compilerFlags(compilerFlags)
	, m_systemHeaderSearchPaths(systemHeaderSearchPaths)
	, m_frameworkSearchPaths(frameworkSearchPaths)
{
}

IndexerCommandCxxCdb::~IndexerCommandCxxCdb()
{
}

std::string IndexerCommandCxxCdb::getKindString() const
{
	return getIndexerKindString();
}

FilePath IndexerCommandCxxCdb::getWorkingDirectory() const
{
	return m_workingDirectory;
}

std::vector<std::string> IndexerCommandCxxCdb::getCompilerFlags() const
{
	return m_compilerFlags;
}

std::vector<FilePath> IndexerCommandCxxCdb::getSystemHeaderSearchPaths() const
{
	return m_systemHeaderSearchPaths;
}

std::vector<FilePath> IndexerCommandCxxCdb::getFrameworkSearchPaths() const
{
	return m_frameworkSearchPaths;
}
