#include "data/indexer/IndexerCommandJava.h"

IndexerCommandType IndexerCommandJava::getStaticIndexerCommandType()
{
	return INDEXER_COMMAND_JAVA;
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

IndexerCommandType IndexerCommandJava::getIndexerCommandType() const
{
	return getStaticIndexerCommandType();
}

size_t IndexerCommandJava::getByteSize() const
{
	size_t size = IndexerCommand::getByteSize();

	for (auto& i : m_classPath)
	{
		size += sizeof(std::string) + i.str().size();
	}

	return size;
}

std::vector<FilePath> IndexerCommandJava::getClassPath() const
{
	return m_classPath;
}
