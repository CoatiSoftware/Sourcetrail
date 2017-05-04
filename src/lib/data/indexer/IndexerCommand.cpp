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

size_t IndexerCommand::getByteSize() const
{
	size_t size = m_sourceFilePath.str().size();

	for (auto i : m_indexedPaths)
	{
		size += i.str().size();
	}

	for (auto i : m_excludedPaths)
	{
		size += i.str().size();
	}

	return size;
}

FilePath IndexerCommand::getSourceFilePath() const
{
	return m_sourceFilePath;
}

std::set<FilePath> IndexerCommand::getIndexedPaths() const
{
	return m_indexedPaths;
}

std::set<FilePath> IndexerCommand::getExcludedPath() const
{
	return m_excludedPaths;
}
