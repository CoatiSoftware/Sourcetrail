#include "data/indexer/IndexerCommandJava.h"

std::string IndexerCommandJava::getIndexerKindString()
{
	return "Java";
}

IndexerCommandJava::IndexerCommandJava(
	const FilePath& sourceFilePath,
	const std::set<FilePath>& indexedPaths,
	const std::set<FilePath>& excludedPaths,
	const std::vector<FilePath>& classPath
)
	: IndexerCommand(sourceFilePath, indexedPaths, excludedPaths)
	, m_classPath(classPath)
{
}

IndexerCommandJava::~IndexerCommandJava()
{
}

std::string IndexerCommandJava::getKindString() const
{
	return getIndexerKindString();
}

std::vector<FilePath> IndexerCommandJava::getClassPath() const
{
	return m_classPath;
}
