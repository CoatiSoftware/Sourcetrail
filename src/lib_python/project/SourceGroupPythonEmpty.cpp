#include "SourceGroupPythonEmpty.h"

#include "FileManager.h"
#include "IndexerCommandCustom.h"
#include "ProjectSettings.h"
#include "ResourcePaths.h"
#include "SourceGroupSettingsPythonEmpty.h"
#include "SqliteIndexStorage.h"
#include "utility.h"

SourceGroupPythonEmpty::SourceGroupPythonEmpty(std::shared_ptr<SourceGroupSettingsPythonEmpty> settings)
	: m_settings(settings)
{
}

std::set<FilePath> SourceGroupPythonEmpty::filterToContainedFilePaths(const std::set<FilePath>& filePaths) const
{
	return SourceGroup::filterToContainedFilePaths(
		filePaths,
		std::set<FilePath>(),
		utility::toSet(m_settings->getSourcePathsExpandedAndAbsolute()),
		m_settings->getExcludeFiltersExpandedAndAbsolute()
	);
}

std::set<FilePath> SourceGroupPythonEmpty::getAllSourceFilePaths() const
{
	FileManager fileManager;
	fileManager.update(
		m_settings->getSourcePathsExpandedAndAbsolute(),
		m_settings->getExcludeFiltersExpandedAndAbsolute(),
		m_settings->getSourceExtensions()
	);
	return fileManager.getAllSourceFilePaths();
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupPythonEmpty::getIndexerCommands(const std::set<FilePath>& filesToIndex) const
{
	std::wstring args = L"";

	args += L" --source-file-path=%{SOURCE_FILE_PATH}";
	args += L" --database-file-path=%{DATABASE_FILE_PATH}";

	if (!m_settings->getEnvironmentDirectoryPath().empty())
	{
		args += L" --environment-directory-path=" + m_settings->getEnvironmentDirectoryPathExpandedAndAbsolute().wstr();
	}

	std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;
	for (const FilePath& sourceFilePath : getAllSourceFilePaths())
	{
		if (filesToIndex.find(sourceFilePath) != filesToIndex.end())
		{
			indexerCommands.push_back(std::make_shared<IndexerCommandCustom>(
				L"\"" + ResourcePaths::getPythonPath().wstr() + L"SourcetrailPythonIndexer\"" + args,
				m_settings->getProjectSettings()->getProjectFilePath(),
				m_settings->getProjectSettings()->getTempDBFilePath(),
				std::to_wstring(SqliteIndexStorage::getStorageVersion()),
				sourceFilePath,
				true
			));
		}
	}

	return indexerCommands;
}

std::shared_ptr<SourceGroupSettings> SourceGroupPythonEmpty::getSourceGroupSettings()
{
	return m_settings;
}

std::shared_ptr<const SourceGroupSettings> SourceGroupPythonEmpty::getSourceGroupSettings() const
{
	return m_settings;
}
