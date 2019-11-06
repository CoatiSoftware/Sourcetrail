#include "SourceGroupJavaSonargraph.h"

#include "Application.h"
#include "ApplicationSettings.h"
#include "IndexerCommandJava.h"
#include "MessageStatus.h"
#include "RefreshInfo.h"
#include "SonargraphProject.h"
#include "SourceGroupSettingsJavaSonargraph.h"
#include "utilityJava.h"

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
		std::wstring error = L"Can't refresh project. The referenced Sonargraph project does not exist anymore: "
			+ sonargraphProjectPath.wstr();
		MessageStatus(error, true).dispatch();
		Application::getInstance()->handleDialog(error, { L"Ok" });
		return false;
	}
	return true;
}

std::set<FilePath> SourceGroupJavaSonargraph::filterToContainedFilePaths(const std::set<FilePath>& filePaths) const
{
	std::set<FilePath> containedFilePaths;

	const std::set<FilePath> indexedPaths = getAllSourceFilePaths();

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

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupJavaSonargraph::getIndexerCommands(const RefreshInfo& info) const
{
	std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;

	if (std::shared_ptr<Sonargraph::Project> project = Sonargraph::Project::load(
		m_settings->getSonargraphProjectPathExpandedAndAbsolute(), getLanguage()
	))
	{
		for (std::shared_ptr<IndexerCommand> indexerCommand : project->getIndexerCommands(m_settings, ApplicationSettings::getInstance()))
		{
			if (info.filesToIndex.find(indexerCommand->getSourceFilePath()) != info.filesToIndex.end())
			{
				indexerCommands.push_back(indexerCommand);
			}
		}
	}

	if (!indexerCommands.empty())
	{
		// TODO: remove this hack once sourcegroup and sourcegroupsettings are merged
		const std::vector<FilePath> classPath = utility::getClassPath(
			m_settings->getClasspathExpandedAndAbsolute(), m_settings->getUseJreSystemLibrary(), getAllSourceFilePaths()
		);
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
