#include "IndexerCommandCustom.h"

#include <QJsonArray>
#include <QJsonObject>

#include "utilityString.h"

IndexerCommandType IndexerCommandCustom::getStaticIndexerCommandType()
{
	return INDEXER_COMMAND_CUSTOM;
}

IndexerCommandCustom::IndexerCommandCustom(
	const std::wstring& customCommand,
	const FilePath& projectFilePath,
	const FilePath& databaseFilePath,
	const std::wstring& databaseVersion,
	const FilePath& sourceFilePath,
	bool runInParallel
)
	: IndexerCommand(sourceFilePath)
	, m_customCommand(customCommand)
	, m_projectFilePath(projectFilePath)
	, m_databaseFilePath(databaseFilePath)
	, m_databaseVersion(databaseVersion)
	, m_runInParallel(runInParallel)
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

FilePath IndexerCommandCustom::getDatabaseFilePath() const
{
	return m_databaseFilePath;
}

void IndexerCommandCustom::setDatabaseFilePath(const FilePath& databaseFilePath)
{
	m_databaseFilePath = databaseFilePath;
}

std::wstring IndexerCommandCustom::getCustomCommand() const
{
	std::wstring command = m_customCommand;

	command = utility::replace(command, L"%{PROJECT_FILE_PATH}", L'\"' + m_projectFilePath.wstr() + L'\"');
	command = utility::replace(command, L"%{DATABASE_FILE_PATH}", L'\"' + m_databaseFilePath.wstr() + L'\"');
	command = utility::replace(command, L"%{DATABASE_VERSION}", L'\"' + m_databaseVersion + L'\"');
	command = utility::replace(command, L"%{SOURCE_FILE_PATH}", L'\"' + getSourceFilePath().wstr() + L'\"');

	return command;
}

bool IndexerCommandCustom::getRunInParallel() const
{
	return m_runInParallel;
}

QJsonObject IndexerCommandCustom::doSerialize() const
{
	QJsonObject jsonObject = IndexerCommand::doSerialize();

	{
		jsonObject["custom_command"] = QString::fromStdWString(m_customCommand);
	}
	{
		jsonObject["run_in_parallel"] = m_runInParallel;
	}

	return jsonObject;
}
