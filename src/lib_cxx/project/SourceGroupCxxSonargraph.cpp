#include "project/SourceGroupCxxSonargraph.h"

#include "data/indexer/IndexerCommand.h"
#include "settings/ApplicationSettings.h"
#include "settings/SourceGroupSettingsCxxSonargraph.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/sonargraph/SonargraphProject.h"
#include "utility/utility.h"
#include "Application.h"

SourceGroupCxxSonargraph::SourceGroupCxxSonargraph(std::shared_ptr<SourceGroupSettingsCxxSonargraph> settings)
	: m_settings(settings)
{
}

bool SourceGroupCxxSonargraph::prepareIndexing()
{
	FilePath sonargraphProjectPath = m_settings->getSonargraphProjectPathExpandedAndAbsolute();
	if (!sonargraphProjectPath.empty() && !sonargraphProjectPath.exists())
	{
		MessageStatus(L"Can't refresh project").dispatch();

		if (std::shared_ptr<Application> application = Application::getInstance())
		{
			if (application->hasGUI())
			{
				application->handleDialog(
					L"Can't refresh. The referenced Sonargraph project does not exist anymore: " + sonargraphProjectPath.wstr(),
					{ L"Ok" }
				);
			}
		}
		return false;
	}
	return true;
}

std::set<FilePath> SourceGroupCxxSonargraph::filterToContainedFilePaths(const std::set<FilePath>& filePaths) const
{
	std::set<FilePath> containedFilePaths;

	const std::set<FilePath> indexedPaths = utility::concat(
		getAllSourceFilePaths(),
		utility::toSet(m_settings->getIndexedHeaderPathsExpandedAndAbsolute())
	);

	for (const FilePath& filePath : filePaths)
	{
		for (const FilePath& indexedPath : indexedPaths)
		{
			if (indexedPath == filePath || indexedPath.contains(filePath))
			{
				containedFilePaths.insert(filePath);
				break;
			}
		}
	}

	return containedFilePaths;
}

std::set<FilePath> SourceGroupCxxSonargraph::getAllSourceFilePaths() const
{
	std::set<FilePath> sourceFilePaths;
	if (std::shared_ptr<Sonargraph::Project> project = Sonargraph::Project::load(
		m_settings->getSonargraphProjectPathExpandedAndAbsolute(), getLanguage()
	))
	{
		for (const FilePath& filePath : project->getAllSourceFilePathsCanonical())
		{
			if (filePath.exists())
			{
				sourceFilePaths.insert(filePath);
			}
		}
	}
	return sourceFilePaths;
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupCxxSonargraph::getIndexerCommands(const std::set<FilePath>& filesToIndex) const
{
	std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;
	if (std::shared_ptr<Sonargraph::Project> project = Sonargraph::Project::load(
		m_settings->getSonargraphProjectPathExpandedAndAbsolute(), getLanguage()
	))
	{
		for (std::shared_ptr<IndexerCommand> indexerCommand : project->getIndexerCommands(m_settings, ApplicationSettings::getInstance()))
		{
			if (filesToIndex.find(indexerCommand->getSourceFilePath()) != filesToIndex.end())
			{
				indexerCommands.push_back(indexerCommand);
			}
		}
	}
	return indexerCommands;
}

std::shared_ptr<SourceGroupSettings> SourceGroupCxxSonargraph::getSourceGroupSettings()
{
	return m_settings;
}

std::shared_ptr<const SourceGroupSettings> SourceGroupCxxSonargraph::getSourceGroupSettings() const
{
	return m_settings;
}
