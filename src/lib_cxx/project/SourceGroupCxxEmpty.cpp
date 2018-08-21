#include "project/SourceGroupCxxEmpty.h"

#include "data/indexer/IndexerCommandCxxEmpty.h"
#include "settings/ApplicationSettings.h"
#include "settings/SourceGroupSettingsCEmpty.h"
#include "settings/SourceGroupSettingsCppEmpty.h"
#include "settings/SourceGroupSettingsWithCppStandard.h"
#include "settings/SourceGroupSettingsWithCStandard.h"
#include "utility/file/FileManager.h"
#include "utility/utility.h"
#include "utility/utilitySourceGroupCxx.h"

SourceGroupCxxEmpty::SourceGroupCxxEmpty(std::shared_ptr<SourceGroupSettingsCxx> settings)
	: m_settings(settings)
{
}

std::set<FilePath> SourceGroupCxxEmpty::filterToContainedFilePaths(const std::set<FilePath>& filePaths) const
{
	std::vector<FilePath> indexedPaths;
	std::vector<FilePathFilter> excludeFilters;
	if (std::shared_ptr<SourceGroupSettingsCEmpty> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsCEmpty>(m_settings))
	{
		indexedPaths = settings->getSourcePathsExpandedAndAbsolute();
		excludeFilters = settings->getExcludeFiltersExpandedAndAbsolute();
	}
	else if (std::shared_ptr<SourceGroupSettingsCppEmpty> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsCppEmpty>(m_settings))
	{
		indexedPaths = settings->getSourcePathsExpandedAndAbsolute();
		excludeFilters = settings->getExcludeFiltersExpandedAndAbsolute();
	}

	return utility::filterToContainedFilePaths(
		filePaths,
		std::set<FilePath>(),
		utility::toSet(indexedPaths),
		excludeFilters
	);
}

std::set<FilePath> SourceGroupCxxEmpty::getAllSourceFilePaths() const
{
	FileManager fileManager;
	if (std::shared_ptr<SourceGroupSettingsCEmpty> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsCEmpty>(m_settings))
	{
		fileManager.update(
			settings->getSourcePathsExpandedAndAbsolute(),
			settings->getExcludeFiltersExpandedAndAbsolute(),
			settings->getSourceExtensions()
		);
	}
	else if (std::shared_ptr<SourceGroupSettingsCppEmpty> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsCppEmpty>(m_settings))
	{
		fileManager.update(
			settings->getSourcePathsExpandedAndAbsolute(),
			settings->getExcludeFiltersExpandedAndAbsolute(),
			settings->getSourceExtensions()
		);
	}

	return fileManager.getAllSourceFilePaths();
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupCxxEmpty::getIndexerCommands(const std::set<FilePath>& filesToIndex) const
{
	std::shared_ptr<ApplicationSettings> appSettings = ApplicationSettings::getInstance();

	std::vector<FilePath> systemHeaderSearchPaths;

	std::set<FilePath> indexedPaths;
	std::set<FilePathFilter> excludeFilters;
	std::wstring targetFlag;
	if (std::shared_ptr<SourceGroupSettingsCEmpty> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsCEmpty>(m_settings))
	{
		indexedPaths = utility::toSet(settings->getSourcePathsExpandedAndAbsolute());
		excludeFilters = utility::toSet(settings->getExcludeFiltersExpandedAndAbsolute());
		targetFlag = settings->getTargetFlag();
	}
	else if (std::shared_ptr<SourceGroupSettingsCppEmpty> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsCppEmpty>(m_settings))
	{
		indexedPaths = utility::toSet(settings->getSourcePathsExpandedAndAbsolute());
		excludeFilters = utility::toSet(settings->getExcludeFiltersExpandedAndAbsolute());
		targetFlag = settings->getTargetFlag();
	}

	// Add the source paths as HeaderSearchPaths as well, so clang will also look here when searching include files.
	for (const FilePath& sourcePath: indexedPaths)
	{
		if (sourcePath.isDirectory())
		{
			systemHeaderSearchPaths.push_back(sourcePath);
		}
	}

	utility::append(systemHeaderSearchPaths, m_settings->getHeaderSearchPathsExpandedAndAbsolute());
	utility::append(systemHeaderSearchPaths, appSettings->getHeaderSearchPathsExpanded());

	std::vector<FilePath> frameworkSearchPaths;
	utility::append(frameworkSearchPaths, m_settings->getFrameworkSearchPathsExpandedAndAbsolute());
	utility::append(frameworkSearchPaths, appSettings->getFrameworkSearchPathsExpanded());

	std::vector<std::wstring> compilerFlags;
	{
		if (!targetFlag.empty())
		{
			compilerFlags.push_back(targetFlag);
		}
	}

	if (std::dynamic_pointer_cast<SourceGroupSettingsCppEmpty>(m_settings))
	{
		compilerFlags.push_back(L"-x");
		compilerFlags.push_back(L"c++");
	}

	utility::append(compilerFlags, m_settings->getCompilerFlags());

	std::string languageStandard = SourceGroupSettingsWithCppStandard::getDefaultCppStandardStatic();
	if (std::shared_ptr<SourceGroupSettingsWithCStandard> cSettings =
		std::dynamic_pointer_cast<SourceGroupSettingsWithCStandard>(m_settings))
	{
		languageStandard = cSettings->getCStandard();
	}
	else if (std::shared_ptr<SourceGroupSettingsWithCppStandard> cppSettings =
		std::dynamic_pointer_cast<SourceGroupSettingsWithCppStandard>(m_settings))
	{
		languageStandard = cppSettings->getCppStandard();
	}
	else
	{
		LOG_ERROR("Source group doesn't specify language standard. Falling back to \"" + languageStandard + "\".");
	}

	std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;
	for (const FilePath& sourcePath: getAllSourceFilePaths())
	{
		if (filesToIndex.find(sourcePath) != filesToIndex.end())
		{
			indexerCommands.push_back(std::make_shared<IndexerCommandCxxEmpty>(
				sourcePath,
				indexedPaths,
				excludeFilters,
				std::set<FilePathFilter>(),
				m_settings->getProjectDirectoryPath(),
				systemHeaderSearchPaths,
				frameworkSearchPaths,
				compilerFlags,
				languageStandard
			));
		}
	}

	return indexerCommands;
}

std::shared_ptr<SourceGroupSettings> SourceGroupCxxEmpty::getSourceGroupSettings()
{
	return m_settings;
}

std::shared_ptr<const SourceGroupSettings> SourceGroupCxxEmpty::getSourceGroupSettings() const
{
	return m_settings;
}
