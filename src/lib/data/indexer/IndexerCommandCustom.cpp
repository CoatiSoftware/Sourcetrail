#include "IndexerCommandCustom.h"

#include <QJsonArray>
#include <QJsonObject>

IndexerCommandType IndexerCommandCustom::getStaticIndexerCommandType()
{
	return INDEXER_COMMAND_CUSTOM;
}

IndexerCommandCustom::IndexerCommandCustom(
	const FilePath& sourceFilePath,
	const std::wstring& customCommand
)
	: IndexerCommand(sourceFilePath)
	, m_customCommand(customCommand)
{
}

IndexerCommandType IndexerCommandCustom::getIndexerCommandType() const
{
	return getStaticIndexerCommandType();
}

size_t IndexerCommandCustom::getByteSize(size_t stringSize) const
{
	size_t size = IndexerCommand::getByteSize(stringSize);

	return size;
}

const std::wstring& IndexerCommandCustom::getCustomCommand() const
{
	return m_customCommand;
}

QJsonObject IndexerCommandCustom::doSerialize() const
{
	QJsonObject jsonObject = IndexerCommand::doSerialize();

	{
		jsonObject["custom_command"] = QString::fromStdWString(m_customCommand);
	}

	return jsonObject;
}
