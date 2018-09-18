#include "SourceGroupCxxEmpty.h"

#include "CxxIndexerCommandProvider.h"
#include "IndexerCommandCxx.h"
#include "ApplicationSettings.h"
#include "SourceGroupSettingsCEmpty.h"
#include "SourceGroupSettingsCppEmpty.h"
#include "SourceGroupSettingsWithCppStandard.h"
#include "SourceGroupSettingsWithCStandard.h"
#include "FileManager.h"
#include "utility.h"
#include "utilitySourceGroupCxx.h"

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

std::shared_ptr<IndexerCommandProvider> SourceGroupCxxEmpty::getIndexerCommandProvider(const std::set<FilePath>& filesToIndex) const
{
	std::shared_ptr<ApplicationSettings> appSettings = ApplicationSettings::getInstance();


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

	std::vector<std::wstring> compilerFlags;
	{
		if (!targetFlag.empty())
		{
			compilerFlags.push_back(targetFlag);
		}
	}

	{
		std::wstring languageStandard = SourceGroupSettingsWithCppStandard::getDefaultCppStandardStatic();
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
			LOG_ERROR(L"Source group doesn't specify language standard. Falling back to \"" + languageStandard + L"\".");
		}

		compilerFlags.push_back(IndexerCommandCxx::getCompilerFlagLanguageStandard(languageStandard));
	}

	if (std::dynamic_pointer_cast<SourceGroupSettingsCppEmpty>(m_settings))
	{
		compilerFlags.push_back(L"-x");
		compilerFlags.push_back(L"c++");
	}

	{
		// Add the source paths as HeaderSearchPaths as well, so clang will also look here when searching include files.
		std::vector<FilePath> indexedDirectoryPaths;
		for (const FilePath& sourcePath : indexedPaths)
		{
			if (sourcePath.isDirectory())
			{
				indexedDirectoryPaths.push_back(sourcePath);
			}
		}

		utility::append(compilerFlags, IndexerCommandCxx::getCompilerFlagsForSystemHeaderSearchPaths(indexedDirectoryPaths));
		utility::append(compilerFlags, IndexerCommandCxx::getCompilerFlagsForSystemHeaderSearchPaths(m_settings->getHeaderSearchPathsExpandedAndAbsolute()));
		utility::append(compilerFlags, IndexerCommandCxx::getCompilerFlagsForSystemHeaderSearchPaths(appSettings->getHeaderSearchPathsExpanded()));
	}
	{
		utility::append(compilerFlags, IndexerCommandCxx::getCompilerFlagsForFrameworkSearchPaths(m_settings->getFrameworkSearchPathsExpandedAndAbsolute()));
		utility::append(compilerFlags, IndexerCommandCxx::getCompilerFlagsForFrameworkSearchPaths(appSettings->getFrameworkSearchPathsExpanded()));
	}

	utility::append(compilerFlags, m_settings->getCompilerFlags());

	std::shared_ptr<CxxIndexerCommandProvider> provider = std::make_shared<CxxIndexerCommandProvider>();
	for (const FilePath& sourcePath: getAllSourceFilePaths())
	{
		if (filesToIndex.find(sourcePath) != filesToIndex.end())
		{
			provider->addCommand(std::make_shared<IndexerCommandCxx>(
				sourcePath,
				indexedPaths,
				excludeFilters,
				std::set<FilePathFilter>(),
				m_settings->getProjectDirectoryPath(),
				utility::concat(compilerFlags, sourcePath.wstr())
			));
		}
	}

	return provider;
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupCxxEmpty::getIndexerCommands(const std::set<FilePath>& filesToIndex) const
{
	return getIndexerCommandProvider(filesToIndex)->consumeAllCommands();
}

std::shared_ptr<SourceGroupSettings> SourceGroupCxxEmpty::getSourceGroupSettings()
{
	return m_settings;
}

std::shared_ptr<const SourceGroupSettings> SourceGroupCxxEmpty::getSourceGroupSettings() const
{
	return m_settings;
}
