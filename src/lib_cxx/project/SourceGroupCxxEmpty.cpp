#include "project/SourceGroupCxxEmpty.h"

#include "data/indexer/IndexerCommandCxxEmpty.h"
#include "settings/ApplicationSettings.h"
#include "settings/SourceGroupSettingsCxxEmpty.h"
#include "settings/SourceGroupSettingsWithCppStandard.h"
#include "settings/SourceGroupSettingsWithCStandard.h"
#include "utility/file/FileManager.h"
#include "utility/utility.h"

SourceGroupCxxEmpty::SourceGroupCxxEmpty(std::shared_ptr<SourceGroupSettingsCxxEmpty> settings)
	: m_settings(settings)
{
}

std::set<FilePath> SourceGroupCxxEmpty::filterToContainedFilePaths(const std::set<FilePath>& filePaths) const
{
	std::set<FilePath> containedFilePaths;

	const std::set<FilePath> allSourceFilePaths = getAllSourceFilePaths();
	const std::vector<FilePath> indexedPaths = m_settings->getSourcePathsExpandedAndAbsolute();
	const std::vector<FilePathFilter> excludeFilters = m_settings->getExcludeFiltersExpandedAndAbsolute();

	for (const FilePath& filePath : filePaths)
	{
		bool isInIndexedPaths = false;
		for (const FilePath& indexedPath : indexedPaths)
		{
			if (indexedPath == filePath || indexedPath.contains(filePath))
			{
				isInIndexedPaths = true;
				break;
			}
		}

		if (isInIndexedPaths)
		{
			for (const FilePathFilter& excludeFilter : excludeFilters)
			{
				if (excludeFilter.isMatching(filePath))
				{
					isInIndexedPaths = false;
					break;
				}
			}
		}

		if (isInIndexedPaths)
		{
			containedFilePaths.insert(filePath);
		}
	}
	return containedFilePaths;
}

std::set<FilePath> SourceGroupCxxEmpty::getAllSourceFilePaths() const
{
	FileManager fileManager;
	fileManager.update(
		m_settings->getSourcePathsExpandedAndAbsolute(),
		m_settings->getExcludeFiltersExpandedAndAbsolute(),
		m_settings->getSourceExtensions()
	);
	return fileManager.getAllSourceFilePaths();
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupCxxEmpty::getIndexerCommands(const std::set<FilePath>& filesToIndex) const
{
	std::shared_ptr<ApplicationSettings> appSettings = ApplicationSettings::getInstance();

	std::vector<FilePath> systemHeaderSearchPaths;

	// Add the source paths as HeaderSearchPaths as well, so clang will also look here when searching include files.
	for (const FilePath& sourcePath: m_settings->getSourcePathsExpandedAndAbsolute())
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
		const std::wstring targetFlag = m_settings->getTargetFlag();
		if (!targetFlag.empty())
		{
			compilerFlags.push_back(targetFlag);
		}
	}

	utility::append(compilerFlags, m_settings->getCompilerFlags());

	const std::set<FilePath> indexedPaths = utility::toSet(m_settings->getSourcePathsExpandedAndAbsolute());
	const std::set<FilePathFilter> excludeFilters = utility::toSet(m_settings->getExcludeFiltersExpandedAndAbsolute());

	std::string languageStandard = SourceGroupSettingsWithCppStandard::getDefaultCppStandardStatic();
	if (std::shared_ptr<SourceGroupSettingsWithCStandard> cSettings =
		std::dynamic_pointer_cast<SourceGroupSettingsWithCStandard>(m_settings))
	{
		languageStandard = cSettings->getCStandard();
	}
	else if (std::shared_ptr<SourceGroupSettingsWithCppStandard> cppSettisngs =
		std::dynamic_pointer_cast<SourceGroupSettingsWithCppStandard>(m_settings))
	{
		languageStandard = cppSettisngs->getCppStandard();
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
