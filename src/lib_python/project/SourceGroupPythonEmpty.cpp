#include "SourceGroupPythonEmpty.h"

#include "../../lib/settings/ApplicationSettings.h"
#include "../../lib/utility/file/FileManager.h"
#include "../../lib/data/indexer/IndexerCommandCustom.h"
#include "../../lib/settings/ProjectSettings.h"
#include "../../lib/project/RefreshInfo.h"
#include "../../lib/app/paths/ResourcePaths.h"
#include "../../lib/settings/source_group/type/SourceGroupSettingsPythonEmpty.h"
#include "../../lib/data/storage/sqlite/SqliteIndexStorage.h"
#include "../../lib/utility/utility.h"

SourceGroupPythonEmpty::SourceGroupPythonEmpty(std::shared_ptr<SourceGroupSettingsPythonEmpty> settings)
	: m_settings(settings)
{
}

bool SourceGroupPythonEmpty::allowsPartialClearing() const
{
	return false;
}

bool SourceGroupPythonEmpty::allowsShallowIndexing() const
{
	return true;
}


std::set<FilePath> SourceGroupPythonEmpty::filterToContainedFilePaths(
	const std::set<FilePath>& filePaths) const
{
	return SourceGroup::filterToContainedFilePaths(
		filePaths,
		std::set<FilePath>(),
		utility::toSet(m_settings->getSourcePathsExpandedAndAbsolute()),
		m_settings->getExcludeFiltersExpandedAndAbsolute());
}

std::set<FilePath> SourceGroupPythonEmpty::getAllSourceFilePaths() const
{
	FileManager fileManager;
	fileManager.update(
		m_settings->getSourcePathsExpandedAndAbsolute(),
		m_settings->getExcludeFiltersExpandedAndAbsolute(),
		m_settings->getSourceExtensions());
	return fileManager.getAllSourceFilePaths();
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupPythonEmpty::getIndexerCommands(
	const RefreshInfo& info) const
{
	std::wstring args = L"";

	args += L" --source-file-path=%{SOURCE_FILE_PATH}";
	args += L" --database-file-path=%{DATABASE_FILE_PATH}";

	if (!m_settings->getEnvironmentPath().empty())
	{
		args += L" --environment-path=\"" + m_settings->getEnvironmentPathExpandedAndAbsolute().wstr() + L"\"";
	}

	if (ApplicationSettings::getInstance()->getVerboseIndexerLoggingEnabled())
	{
		args += L" --verbose";
	}

	if (info.shallow)
	{
		args += L" --shallow";
	}

	std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;
	for (const FilePath& sourceFilePath: getAllSourceFilePaths())
	{
		if (info.filesToIndex.find(sourceFilePath) != info.filesToIndex.end())
		{
			indexerCommands.push_back(std::make_shared<IndexerCommandCustom>(
				INDEXER_COMMAND_PYTHON,
				L"\"" + ResourcePaths::getPythonPath().wstr() +
					L"SourcetrailPythonIndexer\" index" + args,
				m_settings->getProjectSettings()->getProjectFilePath(),
				m_settings->getProjectSettings()->getTempDBFilePath(),
				std::to_wstring(SqliteIndexStorage::getStorageVersion()),
				sourceFilePath,
				true));
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
