#include "Project.h"

#include "utility/file/FileSystem.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/scheduling/TaskGroupSequential.h"
#include "utility/utility.h"
#include "utility/Version.h"

#include "data/access/StorageAccessProxy.h"
#include "data/graph/Token.h"
#include "data/parser/cxx/TaskParseCxx.h"
#include "data/Storage.h"
#include "data/TaskCleanStorage.h"
#include "settings/ApplicationSettings.h"
#include "settings/ProjectSettings.h"

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

void Project::reloadProjectSettings()
{
	if (!m_projectSettingsFilepath.empty())
	{
		ProjectSettings::getInstance()->load(m_projectSettingsFilepath);
		updateFileManager();
	}
}

void Project::clearStorage()
{
	if (m_storage)
	{
		m_storage->clear();
	}
}

void Project::loadStorage()
{
	if (m_storageWasLoaded)
	{
		m_storage->startParsing();
		m_storage->finishParsing();
		MessageFinishedParsing(0, 0, 0, m_storage->getErrorCount()).dispatch();
	}
	else
	{
		parseCode();
	}

	m_storageWasLoaded = true;
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
	utility::append(updatedFilePaths, m_storage->getDependingFilePaths(removedFilePaths));

	std::shared_ptr<TaskGroupSequential> taskGroup = std::make_shared<TaskGroupSequential>();

	std::vector<FilePath> filesToClean;
	filesToClean.insert(filesToClean.end(), removedFilePaths.begin(), removedFilePaths.end());
	filesToClean.insert(filesToClean.end(), updatedFilePaths.begin(), updatedFilePaths.end());

	taskGroup->addTask(std::make_shared<TaskCleanStorage>(m_storage.get(), filesToClean));

	std::vector<FilePath> filesToParse;
	filesToParse.insert(filesToParse.end(), addedFilePaths.begin(), addedFilePaths.end());
	filesToParse.insert(filesToParse.end(), updatedFilePaths.begin(), updatedFilePaths.end());

	taskGroup->addTask(std::make_shared<TaskParseCxx>(
		m_storage.get(),
		&m_fileManager,
		getParserArguments(),
		filesToParse
	));

	Task::dispatch(taskGroup);
}

void Project::logStats() const
{
	m_storage->logStats();
}

void Project::setProjectSettingsFilePath(const FilePath& path)
{
	m_storageWasLoaded = false;

	if (path.empty())
	{
		m_storage.reset();
	}
	else if (m_projectSettingsFilepath != path)
	{
		FilePath dbPath = FilePath(path).replaceExtension("sqlite");
		m_storageWasLoaded = dbPath.exists();
		m_storage = std::make_shared<Storage>(dbPath);

		if (m_storageWasLoaded && m_storage->getVersion().isOlderStorageVersionThan(Version::getApplicationVersion()))
		{
			m_storage->clear();
			m_storageWasLoaded = false;
		}
	}

	m_storageAccessProxy->setSubject(m_storage.get());
	m_projectSettingsFilepath = path;
}

void Project::updateFileManager()
{
	std::shared_ptr<ProjectSettings> projSettings = ProjectSettings::getInstance();

	std::vector<FilePath> sourcePaths(projSettings->getSourcePaths());

	std::vector<std::string> sourceExtensions = projSettings->getSourceExtensions();
	std::vector<std::string> includeExtensions = projSettings->getHeaderExtensions();

	m_fileManager.setPaths(sourcePaths, sourceExtensions, includeExtensions);
}

Parser::Arguments Project::getParserArguments() const
{
	std::shared_ptr<ProjectSettings> projSettings = ProjectSettings::getInstance();
	std::shared_ptr<ApplicationSettings> appSettings = ApplicationSettings::getInstance();

	Parser::Arguments args;

	utility::append(args.compilerFlags, projSettings->getCompilerFlags());
	utility::append(args.compilerFlags, appSettings->getCompilerFlags());

	// Add the source paths as HeaderSearchPaths as well, so clang will also look here when searching include files.
	utility::append(args.systemHeaderSearchPaths, m_fileManager.getSourcePaths());

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
	, m_storageWasLoaded(false)
{
}
