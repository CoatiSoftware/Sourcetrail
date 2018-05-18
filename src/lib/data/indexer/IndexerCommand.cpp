#include "data/indexer/IndexerCommand.h"

#include "utility/utilityString.h"

IndexerCommand::IndexerCommand(
	const FilePath& sourceFilePath
)
	: m_sourceFilePath(sourceFilePath)
{
}

size_t IndexerCommand::getByteSize(size_t stringSize) const
{
	return utility::encodeToUtf8(m_sourceFilePath.wstr()).size();
}

const FilePath& IndexerCommand::getSourceFilePath() const
{
	return m_sourceFilePath;
}
