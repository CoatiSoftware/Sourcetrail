#include "project/SourceGroupCxxEmpty.h"

#include "data/indexer/IndexerCommandCxxManual.h"
#include "settings/ApplicationSettings.h"
#include "utility/utility.h"
#include "Application.h"

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

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupCxxEmpty::getIndexerCommands(
	std::set<FilePath>* filesToIndex, bool fullRefresh)
{
	std::shared_ptr<ApplicationSettings> appSettings = ApplicationSettings::getInstance();

	std::vector<FilePath> systemHeaderSearchPaths;
	utility::append(systemHeaderSearchPaths, m_settings->getHeaderSearchPathsExpandedAndAbsolute());
	utility::append(systemHeaderSearchPaths, appSettings->getHeaderSearchPathsExpanded());

	// Add the source paths as HeaderSearchPaths as well, so clang will also look here when searching include files.
	for (const FilePath& sourcePath: m_settings->getSourcePathsExpandedAndAbsolute())
	{
		if (sourcePath.isDirectory())
		{
			systemHeaderSearchPaths.push_back(sourcePath);
		}
	}

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

	std::set<FilePath> indexedPaths;
	for (const FilePath& p : m_settings->getSourcePathsExpandedAndAbsolute())
	{
		if (p.exists())
		{
			indexedPaths.insert(p);
		}
	}

	std::set<FilePath> excludedPaths;
	for (const FilePath& p: m_settings->getExcludePathsExpandedAndAbsolute())
	{
		if (p.exists())
		{
			excludedPaths.insert(p);
		}
	}

	const std::set<FilePath>& sourceFilePathsToIndex = (fullRefresh ? getAllSourceFilePaths() : getSourceFilePathsToIndex());

	std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;
	for (const FilePath& sourcePath: sourceFilePathsToIndex)
	{
		if (filesToIndex->find(sourcePath) != filesToIndex->end())
		{
			indexerCommands.push_back(std::make_shared<IndexerCommandCxxManual>(
				sourcePath,
				indexedPaths,
				excludedPaths,
				m_settings->getStandard(),
				systemHeaderSearchPaths,
				frameworkSearchPaths,
				compilerFlags,
				m_settings->getShouldApplyAnonymousTypedefTransformation()
			));

			filesToIndex->erase(sourcePath);
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