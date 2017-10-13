#include "data/indexer/IndexerCommand.h"

IndexerCommand::IndexerCommand(
	const FilePath& sourceFilePath, const std::set<FilePath>& indexedPaths, const std::set<FilePath>& excludedPaths
)
	: m_sourceFilePath(sourceFilePath)
	, m_indexedPaths(indexedPaths)
	, m_excludedPaths(excludedPaths)
{
}

IndexerCommand::~IndexerCommand()
{
}

size_t IndexerCommand::getByteSize(size_t stringSize) const
{
	size_t size = m_sourceFilePath.str().size();

	for (const FilePath& path: m_indexedPaths)
	{
		size += stringSize + path.str().size();
	}

	for (const FilePath& path : m_excludedPaths)
	{
		size += stringSize + path.str().size();
	}

	return size;
}

const FilePath& IndexerCommand::getSourceFilePath() const
{
	return m_sourceFilePath;
}

const std::set<FilePath>& IndexerCommand::getIndexedPaths() const
{
	return m_indexedPaths;
}

const std::set<FilePath>& IndexerCommand::getExcludedPath() const
{
	return m_excludedPaths;
}
