#include "SourceGroupCxxCodeblocks.h"

#include "Application.h"
#include "ApplicationSettings.h"
#include "CodeblocksProject.h"
#include "CxxIndexerCommandProvider.h"
#include "IndexerCommandCxx.h"
#include "MessageStatus.h"
#include "SourceGroupSettingsCxxCodeblocks.h"
#include "utility.h"

SourceGroupCxxCodeblocks::SourceGroupCxxCodeblocks(
	std::shared_ptr<SourceGroupSettingsCxxCodeblocks> settings)
	: m_settings(settings)
{
}

bool SourceGroupCxxCodeblocks::prepareIndexing()
{
	FilePath codeblocksProjectPath = m_settings->getCodeblocksProjectPathExpandedAndAbsolute();
	if (!codeblocksProjectPath.empty() && !codeblocksProjectPath.exists())
	{
		std::wstring error =
			L"Can't refresh project. The referenced Code::Blocks project does not exist anymore: " +
			codeblocksProjectPath.wstr();
		MessageStatus(error, true).dispatch();
		Application::getInstance()->handleDialog(error, {L"Ok"});
		return false;
	}
	return true;
}

std::set<FilePath> SourceGroupCxxCodeblocks::filterToContainedFilePaths(
	const std::set<FilePath>& filePaths) const
{
	return SourceGroup::filterToContainedFilePaths(
		filePaths,
		getAllSourceFilePaths(),
		utility::toSet(m_settings->getIndexedHeaderPathsExpandedAndAbsolute()),
		m_settings->getExcludeFiltersExpandedAndAbsolute());
}

std::set<FilePath> SourceGroupCxxCodeblocks::getAllSourceFilePaths() const
{
	std::set<FilePath> sourceFilePaths;
	if (std::shared_ptr<Codeblocks::Project> project = Codeblocks::Project::load(
			m_settings->getCodeblocksProjectPathExpandedAndAbsolute()))
	{
		const std::vector<FilePathFilter> excludeFilters =
			m_settings->getExcludeFiltersExpandedAndAbsolute();

		for (const FilePath& filePath:
			 project->getAllSourceFilePathsCanonical(m_settings->getSourceExtensions()))
		{
			bool isExcluded = FilePathFilter::areMatching(excludeFilters, filePath);
			if (!isExcluded && filePath.exists())
			{
				sourceFilePaths.insert(filePath);
			}
		}
	}
	return sourceFilePaths;
}

std::shared_ptr<IndexerCommandProvider> SourceGroupCxxCodeblocks::getIndexerCommandProvider(
	const RefreshInfo& info) const
{
	std::shared_ptr<CxxIndexerCommandProvider> provider =
		std::make_shared<CxxIndexerCommandProvider>();

	if (std::shared_ptr<Codeblocks::Project> project = Codeblocks::Project::load(
			m_settings->getCodeblocksProjectPathExpandedAndAbsolute()))
	{
		for (std::shared_ptr<IndexerCommandCxx> indexerCommand:
			 project->getIndexerCommands(m_settings, ApplicationSettings::getInstance()))
		{
			if (info.filesToIndex.find(indexerCommand->getSourceFilePath()) != info.filesToIndex.end())
			{
				provider->addCommand(indexerCommand);
			}
		}
	}
	return provider;
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupCxxCodeblocks::getIndexerCommands(
	const RefreshInfo& info) const
{
	return getIndexerCommandProvider(info)->consumeAllCommands();
}

std::shared_ptr<SourceGroupSettings> SourceGroupCxxCodeblocks::getSourceGroupSettings()
{
	return m_settings;
}

std::shared_ptr<const SourceGroupSettings> SourceGroupCxxCodeblocks::getSourceGroupSettings() const
{
	return m_settings;
}
