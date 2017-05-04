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
	, m_preprocessorOnly(false)
{
}

IndexerCommandCxx::~IndexerCommandCxx()
{
}

size_t IndexerCommandCxx::getByteSize() const
{
	size_t size = IndexerCommand::getByteSize();

	for (auto i : m_systemHeaderSearchPaths)
	{
		size += i.str().size();
	}

	for (auto i : m_frameworkSearchPaths)
	{
		size += i.str().size();
	}

	for (auto i : m_compilerFlags)
	{
		size += i.size();
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

bool IndexerCommandCxx::preprocessorOnly() const
{
	return m_preprocessorOnly;
}

void IndexerCommandCxx::setPreprocessorOnly(bool preprocessorOnly)
{
	m_preprocessorOnly = preprocessorOnly;
}
