#include "IndexerCommandCustom.h"

#include <QJsonArray>
#include <QJsonObject>

#include "utilityString.h"

IndexerCommandType IndexerCommandCustom::getStaticIndexerCommandType()
{
	return INDEXER_COMMAND_CUSTOM;
}

IndexerCommandCustom::IndexerCommandCustom(
	const std::wstring& command,
	const std::vector<std::wstring>& arguments,
	const FilePath& projectFilePath,
	const FilePath& databaseFilePath,
	const std::wstring& databaseVersion,
	const FilePath& sourceFilePath,
	bool runInParallel)
	: IndexerCommand(sourceFilePath)
	, m_type(getStaticIndexerCommandType())
	, m_command(command)
	, m_arguments(arguments)
	, m_projectFilePath(projectFilePath)
	, m_databaseFilePath(databaseFilePath)
	, m_databaseVersion(databaseVersion)
	, m_runInParallel(runInParallel)
{
}

IndexerCommandCustom::IndexerCommandCustom(
	IndexerCommandType type,
	const std::wstring& command,
	const std::vector<std::wstring>& arguments,
	const FilePath& projectFilePath,
	const FilePath& databaseFilePath,
	const std::wstring& databaseVersion,
	const FilePath& sourceFilePath,
	bool runInParallel)
	: IndexerCommand(sourceFilePath)
	, m_type(type)
	, m_command(command)
	, m_arguments(arguments)
	, m_projectFilePath(projectFilePath)
	, m_databaseFilePath(databaseFilePath)
	, m_databaseVersion(databaseVersion)
	, m_runInParallel(runInParallel)
{
}

IndexerCommandType IndexerCommandCustom::getIndexerCommandType() const
{
	return m_type;
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

std::wstring IndexerCommandCustom::getCommand() const
{
	return replaceVariables(m_command);
}

std::vector<std::wstring> IndexerCommandCustom::getArguments() const
{
	std::vector<std::wstring> args;
	for (const std::wstring& argument: m_arguments)
	{
		args.push_back(replaceVariables(argument));
	}
	return args;
}

bool IndexerCommandCustom::getRunInParallel() const
{
	return m_runInParallel;
}

QJsonObject IndexerCommandCustom::doSerialize() const
{
	QJsonObject jsonObject = IndexerCommand::doSerialize();

	{
		jsonObject["command"] = QString::fromStdWString(m_command);
	}
	{
		QJsonArray argumentsArray;
		for (const std::wstring& argument: m_arguments)
		{
			argumentsArray.append(QString::fromStdWString(argument));
		}
		jsonObject["arguments"] = argumentsArray;
	}
	{
		jsonObject["run_in_parallel"] = m_runInParallel;
	}

	return jsonObject;
}

std::wstring IndexerCommandCustom::replaceVariables(std::wstring s) const
{
	s = utility::replace(s, L"%{PROJECT_FILE_PATH}", m_projectFilePath.wstr());
	s = utility::replace(s, L"%{DATABASE_FILE_PATH}", m_databaseFilePath.wstr());
	s = utility::replace(s, L"%{DATABASE_VERSION}", m_databaseVersion);
	s = utility::replace(s, L"%{SOURCE_FILE_PATH}", getSourceFilePath().wstr());
	return s;
}
