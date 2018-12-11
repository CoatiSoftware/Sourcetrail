#include "SourceGroupCustomCommand.h"

#include "FileManager.h"
#include "IndexerCommandCustom.h"
#include "ProjectSettings.h"
#include "SourceGroupSettingsCustomCommand.h"
#include "SqliteIndexStorage.h"
#include "utility.h"

SourceGroupCustomCommand::SourceGroupCustomCommand(std::shared_ptr<SourceGroupSettingsCustomCommand> settings)
	: m_settings(settings)
{
}

bool SourceGroupCustomCommand::allowsPartialClearing() const
{
	return false;
}

std::set<FilePath> SourceGroupCustomCommand::filterToContainedFilePaths(const std::set<FilePath>& filePaths) const
{
	return SourceGroup::filterToContainedFilePaths(
		filePaths,
		std::set<FilePath>(),
		utility::toSet(m_settings->getSourcePathsExpandedAndAbsolute()),
		m_settings->getExcludeFiltersExpandedAndAbsolute()
	);
}

std::set<FilePath> SourceGroupCustomCommand::getAllSourceFilePaths() const
{
	FileManager fileManager;
	fileManager.update(
		m_settings->getSourcePathsExpandedAndAbsolute(),
		m_settings->getExcludeFiltersExpandedAndAbsolute(),
		m_settings->getSourceExtensions()
	);
	return fileManager.getAllSourceFilePaths();
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupCustomCommand::getIndexerCommands(const std::set<FilePath>& filesToIndex) const
{
	std::wstring customCommand = m_settings->getCustomCommand();

	customCommand = utility::replace(customCommand, L"$PROJECT_PATH", L'\"' + m_settings->getProjectSettings()->getProjectFilePath().wstr() + L'\"');
	customCommand = utility::replace(customCommand, L"$DB_PATH", L'\"' + m_settings->getProjectSettings()->getTempDBFilePath().wstr() + L'\"');
	customCommand = utility::replace(customCommand, L"$STORAGE_VERSION", L'\"' + std::to_wstring(SqliteIndexStorage::getStorageVersion()) + L'\"');

	std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;
	for (const FilePath& sourcePath: getAllSourceFilePaths())
	{
		if (filesToIndex.find(sourcePath) != filesToIndex.end())
		{
			std::wstring command = utility::replace(customCommand, L"$SOURCE_PATH", L'\"' + sourcePath.wstr() + L'\"');

			indexerCommands.push_back(std::make_shared<IndexerCommandCustom>(sourcePath, command));
		}
	}

	return indexerCommands;
}

std::shared_ptr<SourceGroupSettings> SourceGroupCustomCommand::getSourceGroupSettings()
{
	return m_settings;
}

std::shared_ptr<const SourceGroupSettings> SourceGroupCustomCommand::getSourceGroupSettings() const
{
	return m_settings;
}

