#include "data/indexer/IndexerCommandJava.h"

#include "utility/utilityString.h"

IndexerCommandType IndexerCommandJava::getStaticIndexerCommandType()
{
	return INDEXER_COMMAND_JAVA;
}

IndexerCommandJava::IndexerCommandJava(
	const FilePath& sourceFilePath,
	const std::string& languageStandard,
	const std::vector<FilePath>& classPath
)
	: IndexerCommand(sourceFilePath)
	, m_languageStandard(languageStandard)
	, m_classPath(classPath)
{
}

IndexerCommandType IndexerCommandJava::getIndexerCommandType() const
{
	return getStaticIndexerCommandType();
}

size_t IndexerCommandJava::getByteSize(size_t stringSize) const
{
	size_t size = IndexerCommand::getByteSize(stringSize);

	for (const FilePath& i : m_classPath)
	{
		size += stringSize + utility::encodeToUtf8(i.wstr()).size();
	}

	return size;
}

std::string IndexerCommandJava::getLanguageStandard() const
{
	return m_languageStandard;
}

void IndexerCommandJava::setClassPath(std::vector<FilePath> classPath)
{
	m_classPath = classPath;
}

std::vector<FilePath> IndexerCommandJava::getClassPath() const
{
	return m_classPath;
}
