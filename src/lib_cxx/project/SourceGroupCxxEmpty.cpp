#include "project/SourceGroupCxxEmpty.h"

#include "data/indexer/IndexerCommandCxxEmpty.h"
#include "settings/ApplicationSettings.h"
#include "utility/utility.h"

SourceGroupCxxEmpty::SourceGroupCxxEmpty(std::shared_ptr<SourceGroupSettingsCxxEmpty> settings)
	: m_settings(settings)
{
}

SourceGroupCxxEmpty::~SourceGroupCxxEmpty()
{
}

SourceGroupType SourceGroupCxxEmpty::getType() const
{
	return m_settings->getType(); // may be either C or Cpp
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupCxxEmpty::getIndexerCommands(const std::set<FilePath>& filesToIndex)
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

	std::vector<std::string> compilerFlags;
	{
		const std::string targetFlag = m_settings->getTargetFlag();
		if (!targetFlag.empty())
		{
			compilerFlags.push_back(targetFlag);
		}
	}

	utility::append(compilerFlags, m_settings->getCompilerFlags());

	std::set<FilePath> indexedPaths = getIndexedPaths();
	std::set<FilePath> excludedPaths = getExcludedPaths();

	std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;
	for (const FilePath& sourcePath: getAllSourceFilePaths())
	{
		if (filesToIndex.find(sourcePath) != filesToIndex.end())
		{
			indexerCommands.push_back(std::make_shared<IndexerCommandCxxEmpty>(
				sourcePath,
				indexedPaths,
				excludedPaths,
				m_settings->getStandard(),
				systemHeaderSearchPaths,
				frameworkSearchPaths,
				compilerFlags
			));
		}
	}

	return indexerCommands;
}

std::shared_ptr<SourceGroupSettingsCxx> SourceGroupCxxEmpty::getSourceGroupSettingsCxx()
{
	return m_settings;
}

std::vector<FilePath> SourceGroupCxxEmpty::getAllSourcePaths() const
{
	return m_settings->getSourcePathsExpandedAndAbsolute();
}