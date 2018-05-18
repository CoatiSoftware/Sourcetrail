#include "project/SourceGroupJavaSonargraph.h"

#include "data/indexer/IndexerCommandJava.h"
#include "settings/ApplicationSettings.h"
#include "settings/SourceGroupSettingsJavaSonargraph.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/sonargraph/SonargraphProject.h"
#include "utility/utilityJava.h"
#include "Application.h"

SourceGroupJavaSonargraph::SourceGroupJavaSonargraph(std::shared_ptr<SourceGroupSettingsJavaSonargraph> settings)
	: m_settings(settings)
{
}

bool SourceGroupJavaSonargraph::prepareIndexing()
{
	if (!utility::prepareJavaEnvironmentAndDisplayOccurringErrors())
	{
		return false;
	}

	FilePath sonargraphProjectPath = m_settings->getSonargraphProjectPathExpandedAndAbsolute();
	if (!sonargraphProjectPath.empty() && !sonargraphProjectPath.exists())
	{
		MessageStatus(L"Can't refresh project. The referenced Sonargraph project does not exist anymore.").dispatch();

		if (std::shared_ptr<Application> application = Application::getInstance())
		{
			if (application->hasGUI())
			{
				application->handleDialog(
					L"Can't refresh project. The referenced Sonargraph project does not exist anymore: " + sonargraphProjectPath.wstr(),
					{ L"Ok" }
				);
			}
		}
		return false;
	}
	return true;
}

std::set<FilePath> SourceGroupJavaSonargraph::filterToContainedFilePaths(const std::set<FilePath>& filePaths) const
{
	if (std::shared_ptr<Sonargraph::Project> project = Sonargraph::Project::load(
		m_settings->getSonargraphProjectPathExpandedAndAbsolute(), getLanguage()
	))
	{
		return project->filterToContainedFilePaths(filePaths);
	}
	else
	{
		LOG_ERROR("Unable to load Sonargraph project to check the containment of file paths.");
	}
	return std::set<FilePath>();
}

std::set<FilePath> SourceGroupJavaSonargraph::getAllSourceFilePaths() const
{
	if (std::shared_ptr<Sonargraph::Project> project = Sonargraph::Project::load(
		m_settings->getSonargraphProjectPathExpandedAndAbsolute(), getLanguage()
	))
	{
		// DONT CALL getIndexerCommands here because this would create an endless recursion due to hack...
		return project->getAllSourceFilePathsCanonical();
	}
	return std::set<FilePath>();
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupJavaSonargraph::getIndexerCommands(const std::set<FilePath>& filesToIndex) const
{
	std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;
	
	if (std::shared_ptr<Sonargraph::Project> project = Sonargraph::Project::load(
		m_settings->getSonargraphProjectPathExpandedAndAbsolute(), getLanguage()
	))
	{
		indexerCommands = project->getIndexerCommands(m_settings, ApplicationSettings::getInstance());
	}

	if (!indexerCommands.empty())
	{
		const std::vector<FilePath> classPath = utility::getClassPath(m_settings, getAllSourceFilePaths()); // TODO: remove this hack once sourcegroup and sourcegroupsettings are merged
		for (std::shared_ptr<IndexerCommand> indexerCommand : indexerCommands)
		{
			if (std::shared_ptr<IndexerCommandJava> javaIndexerCommand = std::dynamic_pointer_cast<IndexerCommandJava>(indexerCommand))
			{
				javaIndexerCommand->setClassPath(classPath);
			}
		}
	}

	return indexerCommands;
}

std::shared_ptr<SourceGroupSettings> SourceGroupJavaSonargraph::getSourceGroupSettings()
{
	return m_settings;
}

std::shared_ptr<const SourceGroupSettings> SourceGroupJavaSonargraph::getSourceGroupSettings() const
{
	return m_settings;
}
