#include "SourceGroupCustomCommand.h"

#include "FileManager.h"
#include "IndexerCommandCustom.h"
#include "ProjectSettings.h"
#include "RefreshInfo.h"
#include "SourceGroupSettingsCustomCommand.h"
#include "SqliteIndexStorage.h"
#include "utility.h"

SourceGroupCustomCommand::SourceGroupCustomCommand(
	std::shared_ptr<SourceGroupSettingsCustomCommand> settings)
	: m_settings(settings)
{
}

bool SourceGroupCustomCommand::allowsPartialClearing() const
{
	return false;
}

std::set<FilePath> SourceGroupCustomCommand::filterToContainedFilePaths(
	const std::set<FilePath>& filePaths) const
{
	return SourceGroup::filterToContainedFilePaths(
		filePaths,
		std::set<FilePath>(),
		utility::toSet(m_settings->getSourcePathsExpandedAndAbsolute()),
		m_settings->getExcludeFiltersExpandedAndAbsolute());
}

std::set<FilePath> SourceGroupCustomCommand::getAllSourceFilePaths() const
{
	FileManager fileManager;
	fileManager.update(
		m_settings->getSourcePathsExpandedAndAbsolute(),
		m_settings->getExcludeFiltersExpandedAndAbsolute(),
		m_settings->getSourceExtensions());
	return fileManager.getAllSourceFilePaths();
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupCustomCommand::getIndexerCommands(
	const RefreshInfo& info) const
{
	const bool runInParallel = m_settings->getRunInParallel();

	std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;
	for (const FilePath& sourcePath: getAllSourceFilePaths())
	{
		if (info.filesToIndex.find(sourcePath) != info.filesToIndex.end())
		{
			indexerCommands.push_back(std::make_shared<IndexerCommandCustom>(
				m_settings->getCustomCommand(),
				std::vector<std::wstring> {},
				m_settings->getProjectSettings()->getProjectFilePath(),
				m_settings->getProjectSettings()->getTempDBFilePath(),
				std::to_wstring(SqliteIndexStorage::getStorageVersion()),
				sourcePath,
				runInParallel));
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
