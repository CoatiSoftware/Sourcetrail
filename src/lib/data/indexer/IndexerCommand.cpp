#include "data/indexer/IndexerCommand.h"

IndexerCommand::IndexerCommand(const FilePath& sourceFilePath, const std::set<FilePath>& indexedPaths, const std::set<FilePath>& excludedPaths)
	: m_sourceFilePath(sourceFilePath)
	, m_indexedPaths(indexedPaths)
	, m_excludedPaths(excludedPaths)
{
}


IndexerCommand::~IndexerCommand()
{
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
