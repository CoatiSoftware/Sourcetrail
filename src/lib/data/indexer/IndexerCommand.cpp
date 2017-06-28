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

	for (const FilePath& path: m_indexedPaths)
	{
		size += sizeof(std::string) + path.str().size();
	}

	for (const FilePath& path : m_excludedPaths)
	{
		size += sizeof(std::string) + path.str().size();
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
