#include "data/indexer/IndexerCommand.h"

#include "utility/utilityString.h"

IndexerCommand::IndexerCommand(
	const FilePath& sourceFilePath, const std::set<FilePath>& indexedPaths, 
	const std::set<FilePathFilter>& excludeFilters
)
	: m_sourceFilePath(sourceFilePath)
	, m_indexedPaths(indexedPaths)
	, m_excludeFilters(excludeFilters)
{
}

IndexerCommand::~IndexerCommand()
{
}

size_t IndexerCommand::getByteSize(size_t stringSize) const
{
	size_t size = utility::encodeToUtf8(m_sourceFilePath.wstr()).size();

	for (const FilePath& path: m_indexedPaths)
	{
		size += stringSize + utility::encodeToUtf8(path.wstr()).size();
	}

	for (const FilePathFilter& filter : m_excludeFilters)
	{
		size += stringSize + utility::encodeToUtf8(filter.wstr()).size();
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

const std::set<FilePathFilter>& IndexerCommand::getExcludeFilters() const
{
	return m_excludeFilters;
}
