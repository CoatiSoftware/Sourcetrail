#include "project/SourceGroupCxxCodeblocks.h"

#include "data/indexer/CxxIndexerCommandProvider.h"
#include "data/indexer/IndexerCommandCxx.h"
#include "settings/ApplicationSettings.h"
#include "settings/SourceGroupSettingsCxxCodeblocks.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/codeblocks/CodeblocksProject.h"
#include "utility/utility.h"
#include "utility/utilitySourceGroupCxx.h"
#include "Application.h"

SourceGroupCxxCodeblocks::SourceGroupCxxCodeblocks(std::shared_ptr<SourceGroupSettingsCxxCodeblocks> settings)
	: m_settings(settings)
{
}

bool SourceGroupCxxCodeblocks::prepareIndexing()
{
	FilePath codeblocksProjectPath = m_settings->getCodeblocksProjectPathExpandedAndAbsolute();
	if (!codeblocksProjectPath.empty() && !codeblocksProjectPath.exists())
	{
		MessageStatus(L"Can't refresh project").dispatch();

		if (std::shared_ptr<Application> application = Application::getInstance())
		{
			if (application->hasGUI())
			{
				application->handleDialog(
					L"Can't refresh. The referenced Code::Blocks project does not exist anymore: " + codeblocksProjectPath.wstr(),
					{ L"Ok" }
				);
			}
		}
		return false;
	}
	return true;
}

std::set<FilePath> SourceGroupCxxCodeblocks::filterToContainedFilePaths(const std::set<FilePath>& filePaths) const
{
	return utility::filterToContainedFilePaths(
		filePaths,
		getAllSourceFilePaths(),
		utility::toSet(m_settings->getIndexedHeaderPathsExpandedAndAbsolute()),
		m_settings->getExcludeFiltersExpandedAndAbsolute()
	);
}

std::set<FilePath> SourceGroupCxxCodeblocks::getAllSourceFilePaths() const
{
	std::set<FilePath> sourceFilePaths;
	if (std::shared_ptr<Codeblocks::Project> project = Codeblocks::Project::load(
		m_settings->getCodeblocksProjectPathExpandedAndAbsolute()
	))
	{
		const std::vector<FilePathFilter> excludeFilters = m_settings->getExcludeFiltersExpandedAndAbsolute();

		for (const FilePath& filePath : project->getAllSourceFilePathsCanonical(m_settings->getSourceExtensions()))
		{
			bool isExcluded = false;
			for (const FilePathFilter& excludeFilter : excludeFilters)
			{
				if (excludeFilter.isMatching(filePath))
				{
					isExcluded = true;
					break;
				}
			}

			if (!isExcluded && filePath.exists())
			{
				sourceFilePaths.insert(filePath);
			}
		}
	}
	return sourceFilePaths;
}

std::shared_ptr<IndexerCommandProvider> SourceGroupCxxCodeblocks::getIndexerCommandProvider(const std::set<FilePath>& filesToIndex) const
{
	std::shared_ptr<CxxIndexerCommandProvider> provider = std::make_shared<CxxIndexerCommandProvider>();

	if (std::shared_ptr<Codeblocks::Project> project = Codeblocks::Project::load(
		m_settings->getCodeblocksProjectPathExpandedAndAbsolute()
	))
	{
		for (std::shared_ptr<IndexerCommandCxx> indexerCommand: project->getIndexerCommands(m_settings, ApplicationSettings::getInstance()))
		{
			if (filesToIndex.find(indexerCommand->getSourceFilePath()) != filesToIndex.end())
			{
				provider->addCommand(indexerCommand);
			}
		}
	}
	return provider;
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupCxxCodeblocks::getIndexerCommands(const std::set<FilePath>& filesToIndex) const
{
	return getIndexerCommandProvider(filesToIndex)->consumeAllCommands();
}

std::shared_ptr<SourceGroupSettings> SourceGroupCxxCodeblocks::getSourceGroupSettings()
{
	return m_settings;
}

std::shared_ptr<const SourceGroupSettings> SourceGroupCxxCodeblocks::getSourceGroupSettings() const
{
	return m_settings;
}
