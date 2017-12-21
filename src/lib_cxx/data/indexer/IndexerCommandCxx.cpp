#include "data/indexer/IndexerCommandCxx.h"

IndexerCommandCxx::IndexerCommandCxx(
	const FilePath& sourceFilePath,
	const std::set<FilePath>& indexedPaths,
	const std::set<FilePath>& excludedPaths,
	const std::vector<FilePath>& systemHeaderSearchPaths,
	const std::vector<FilePath>& frameworkSearchPaths,
	const std::vector<std::string>& compilerFlags
)
	: IndexerCommand(sourceFilePath, indexedPaths, excludedPaths)
	, m_systemHeaderSearchPaths(systemHeaderSearchPaths)
	, m_frameworkSearchPaths(frameworkSearchPaths)
	, m_compilerFlags(compilerFlags)
{
}

IndexerCommandCxx::~IndexerCommandCxx()
{
}

size_t IndexerCommandCxx::getByteSize(size_t stringSize) const
{
	size_t size = IndexerCommand::getByteSize(stringSize);

	for (auto& i : m_systemHeaderSearchPaths)
	{
		size += stringSize + i.str().size();
	}

	for (auto& i : m_frameworkSearchPaths)
	{
		size += stringSize + i.str().size();
	}

	for (auto& i : m_compilerFlags)
	{
		size += stringSize + i.size();
	}

	return size;
}

std::vector<FilePath> IndexerCommandCxx::getSystemHeaderSearchPaths() const
{
	return m_systemHeaderSearchPaths;
}

std::vector<FilePath> IndexerCommandCxx::getFrameworkSearchPaths() const
{
	return m_frameworkSearchPaths;
}

std::vector<std::string> IndexerCommandCxx::getCompilerFlags() const
{
	return m_compilerFlags;
}
