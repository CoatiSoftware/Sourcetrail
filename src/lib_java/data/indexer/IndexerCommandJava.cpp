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

size_t IndexerCommandJava::getByteSize() const
{
	size_t size = IndexerCommand::getByteSize() + sizeof(*this);

	for (auto i : m_classPath)
	{
		size += i.str().size();
	}

	return size;
}

std::vector<FilePath> IndexerCommandJava::getClassPath() const
{
	return m_classPath;
}

bool IndexerCommandJava::preprocessorOnly() const
{
	return false;
}

void IndexerCommandJava::setPreprocessorOnly(bool preprocessorOnly)
{
}
