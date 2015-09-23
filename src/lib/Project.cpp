#include "Project.h"

#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/utility.h"

#include "data/access/StorageAccessProxy.h"
#include "data/graph/Token.h"
#include "data/parser/cxx/TaskParseCxx.h"
#include "settings/ApplicationSettings.h"
#include "settings/ProjectSettings.h"
#include "utility/file/FileSystem.h"

std::shared_ptr<Project> Project::create(StorageAccessProxy* storageAccessProxy)
{
	std::shared_ptr<Project> ptr(new Project(storageAccessProxy));
	return ptr;
}

Project::~Project()
{
}

bool Project::loadProjectSettings(const FilePath& projectSettingsFile)
{
	bool success = ProjectSettings::getInstance()->load(projectSettingsFile);
	if (success)
	{
		setProjectSettingsFilePath(projectSettingsFile);

		m_fileManager.reset();
		updateFileManager();
	}
	return success;
}

bool Project::saveProjectSettings(const FilePath& projectSettingsFile)
{
	if (!projectSettingsFile.empty())
	{
		setProjectSettingsFilePath(projectSettingsFile);
	}

	if (m_projectSettingsFilepath.empty())
	{
		return false;
	}

	ProjectSettings::getInstance()->save(m_projectSettingsFilepath);

	LOG_INFO_STREAM(<< "ProjectSettings saved to file: " << m_projectSettingsFilepath.str());

	return true;
}

void Project::clearProjectSettings()
{
	setProjectSettingsFilePath(FilePath());
	ProjectSettings::getInstance()->clear();

	m_fileManager.reset();
}

void Project::reloadProjectSettings()
{
	if (!m_projectSettingsFilepath.empty())
	{
		ProjectSettings::getInstance()->load(m_projectSettingsFilepath);
		updateFileManager();
	}
}

void Project::parseCode()
{
	if (m_projectSettingsFilepath.empty())
	{
		return;
	}

	std::shared_ptr<ProjectSettings> projSettings = ProjectSettings::getInstance();

	m_fileManager.fetchFilePaths(m_storage->getInfoOnAllFiles());
	std::set<FilePath> addedFilePaths = m_fileManager.getAddedFilePaths();
	std::set<FilePath> updatedFilePaths = m_fileManager.getUpdatedFilePaths();
	std::set<FilePath> removedFilePaths = m_fileManager.getRemovedFilePaths();

	utility::append(updatedFilePaths, m_storage->getDependingFilePaths(updatedFilePaths));
	utility::append(removedFilePaths, m_storage->getDependingFilePaths(removedFilePaths));

	MessageStatus("Clearing updated files").dispatch();
	m_storage->clearFileElements(updatedFilePaths);

	MessageStatus("Clearing removed files").dispatch();
	m_storage->clearFileElements(removedFilePaths);

	MessageStatus("Cleaning up names").dispatch();
	m_storage->removeUnusedNames();

	std::vector<FilePath> filesToParse;
	filesToParse.insert(filesToParse.end(), addedFilePaths.begin(), addedFilePaths.end());
	filesToParse.insert(filesToParse.end(), updatedFilePaths.begin(), updatedFilePaths.end());

	Task::dispatch(std::make_shared<TaskParseCxx>(
		m_storage.get(),
		&m_fileManager,
		getParserArguments(),
		filesToParse
	));
}

void Project::logStats() const
{
	// m_storage->logGraph();
	// m_storage->logLocations();
	m_storage->logStats();
}

void Project::setProjectSettingsFilePath(const FilePath& path)
{
	if (path.empty())
	{
		m_storage.reset();
	}
	else if (m_projectSettingsFilepath != path)
	{
		m_storage = std::make_shared<Storage>(FilePath(path).replaceExtension("sqlite"));
	}

	m_storageAccessProxy->setSubject(m_storage.get());
	m_projectSettingsFilepath = path;
}

void Project::updateFileManager()
{
	std::shared_ptr<ProjectSettings> projSettings = ProjectSettings::getInstance();

	std::vector<FilePath> sourcePaths(projSettings->getSourcePaths());
	std::vector<FilePath> includePaths(sourcePaths);

	std::vector<std::string> sourceExtensions = projSettings->getSourceExtensions();
	std::vector<std::string> includeExtensions = projSettings->getHeaderExtensions();

	m_fileManager.setPaths(sourcePaths, includePaths, sourceExtensions, includeExtensions);
}

Parser::Arguments Project::getParserArguments() const
{
	std::shared_ptr<ProjectSettings> projSettings = ProjectSettings::getInstance();
	std::shared_ptr<ApplicationSettings> appSettings = ApplicationSettings::getInstance();

	Parser::Arguments args;

	utility::append(args.compilerFlags, projSettings->getCompilerFlags());
	utility::append(args.compilerFlags, appSettings->getCompilerFlags());

	// Add the include paths as HeaderSearchPaths as well, so clang will also look here when searching include files.
	utility::append(args.systemHeaderSearchPaths, m_fileManager.getIncludePaths());

	// std::vector<FilePath> headerSearchSubPaths;
	// for(FilePath p : projSettings->getHeaderSearchPaths())
	// {
	// 	std::vector<FilePath> tempPaths = FileSystem::getSubDirectories(p);
	// 	headerSearchSubPaths.insert( headerSearchSubPaths.end(), tempPaths.begin(), tempPaths.end() );
	// }

	// std::unique(headerSearchSubPaths.begin(),headerSearchSubPaths.end());
	// utility::append(args.systemHeaderSearchPaths, headerSearchSubPaths);

	utility::append(args.systemHeaderSearchPaths, projSettings->getHeaderSearchPaths());
	utility::append(args.systemHeaderSearchPaths, appSettings->getHeaderSearchPaths());

	utility::append(args.frameworkSearchPaths, projSettings->getFrameworkSearchPaths());
	utility::append(args.frameworkSearchPaths, appSettings->getFrameworkSearchPaths());

	return args;
}

Project::Project(StorageAccessProxy* storageAccessProxy)
	: m_storageAccessProxy(storageAccessProxy)
{
}
