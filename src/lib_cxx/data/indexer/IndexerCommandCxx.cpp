#include "data/indexer/IndexerCommandCxx.h"

#include "utility/utilityString.h"

IndexerCommandCxx::IndexerCommandCxx(
	const FilePath& sourceFilePath,
	const std::set<FilePath>& indexedPaths,
	const std::set<FilePath>& excludedPaths,
	const FilePath& workingDirectory,
	const std::vector<FilePath>& systemHeaderSearchPaths,
	const std::vector<FilePath>& frameworkSearchPaths,
	const std::vector<std::wstring>& compilerFlags
)
	: IndexerCommand(sourceFilePath, indexedPaths, excludedPaths)
	, m_workingDirectory(workingDirectory)
	, m_systemHeaderSearchPaths(systemHeaderSearchPaths)
	, m_frameworkSearchPaths(frameworkSearchPaths)
	, m_compilerFlags(compilerFlags)
{
}

size_t IndexerCommandCxx::getByteSize(size_t stringSize) const
{
	size_t size = IndexerCommand::getByteSize(stringSize);

	for (auto& i : m_systemHeaderSearchPaths)
	{
		size += stringSize + utility::encodeToUtf8(i.wstr()).size();
	}

	for (auto& i : m_frameworkSearchPaths)
	{
		size += stringSize + utility::encodeToUtf8(i.wstr()).size();
	}

	for (auto& i : m_compilerFlags)
	{
		size += stringSize + i.size();
	}

	return size;
}

const std::vector<FilePath>& IndexerCommandCxx::getSystemHeaderSearchPaths() const
{
	return m_systemHeaderSearchPaths;
}

const std::vector<FilePath>& IndexerCommandCxx::getFrameworkSearchPaths() const
{
	return m_frameworkSearchPaths;
}

const std::vector<std::wstring>& IndexerCommandCxx::getCompilerFlags() const
{
	return m_compilerFlags;
}

const FilePath& IndexerCommandCxx::getWorkingDirectory() const
{
	return m_workingDirectory;
}
