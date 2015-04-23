#include "Project.h"

#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/utility.h"

#include "data/access/StorageAccessProxy.h"
#include "data/graph/Token.h"
#include "data/parser/cxx/TaskParseCxx.h"
#include "settings/ApplicationSettings.h"
#include "settings/ProjectSettings.h"

std::shared_ptr<Project> Project::create(StorageAccessProxy* storageAccessProxy)
{
	std::shared_ptr<Project> ptr(new Project(storageAccessProxy));
	ptr->clearStorage();
	return ptr;
}

Project::~Project()
{
}

bool Project::loadProjectSettings(const std::string& projectSettingsFile)
{
	bool success = ProjectSettings::getInstance()->load(projectSettingsFile);
	if (success)
	{
		m_projectSettingsFilepath = projectSettingsFile;

		createFileManager();
	}
	return success;
}

bool Project::saveProjectSettings(const std::string& projectSettingsFile)
{
	if (projectSettingsFile.size())
	{
		m_projectSettingsFilepath = projectSettingsFile;
	}

	if (!m_projectSettingsFilepath.size())
	{
		return false;
	}

	ProjectSettings::getInstance()->save(m_projectSettingsFilepath);

	LOG_INFO_STREAM(<< "ProjectSettings saved to file: " << m_projectSettingsFilepath);

	return true;
}

void Project::clearProjectSettings()
{
	m_projectSettingsFilepath.clear();
	ProjectSettings::getInstance()->clear();

	m_fileManager.reset();
}

bool Project::setSourceDirectoryPath(const std::string& sourceDirectoryPath)
{
	m_projectSettingsFilepath = sourceDirectoryPath + "/ProjectSettings.xml";
	bool success = ProjectSettings::getInstance()->setSourcePaths(std::vector<std::string>(1, sourceDirectoryPath));

	if (success)
	{
		createFileManager();
	}

	return success;
}

void Project::clearStorage()
{
	m_storage = std::make_shared<Storage>();
	m_storageAccessProxy->setSubject(m_storage.get());

	Token::resetNextId();
}

void Project::parseCode()
{
	if (!m_fileManager)
	{
		LOG_ERROR("No FileManger was created.");
		return;
	}

	std::shared_ptr<ProjectSettings> projSettings = ProjectSettings::getInstance();

	m_fileManager->fetchFilePaths();
	std::set<FilePath> addedFilePaths = m_fileManager->getAddedFilePaths();
	std::set<FilePath> updatedFilePaths = m_fileManager->getUpdatedFilePaths();
	std::set<FilePath> removedFilePaths = m_fileManager->getRemovedFilePaths();

	utility::append(updatedFilePaths, m_storage->getDependingFilePathsAndRemoveFileNodes(updatedFilePaths));
	utility::append(updatedFilePaths, m_storage->getDependingFilePathsAndRemoveFileNodes(removedFilePaths));

	m_storage->clearFileData(updatedFilePaths);
	m_storage->clearFileData(removedFilePaths);

	std::vector<FilePath> filesToParse;
	filesToParse.insert(filesToParse.end(), addedFilePaths.begin(), addedFilePaths.end());
	filesToParse.insert(filesToParse.end(), updatedFilePaths.begin(), updatedFilePaths.end());

	if (filesToParse.size() == 0)
	{
		MessageFinishedParsing(0, 0, 0, m_storage->getErrorCount()).dispatch();
		return;
	}

	Task::dispatch(std::make_shared<TaskParseCxx>(
		m_storage.get(),
		m_fileManager.get(),
		getParserArguments(),
		filesToParse
	));
}

void Project::createFileManager()
{
	std::shared_ptr<ProjectSettings> projSettings = ProjectSettings::getInstance();

	std::vector<std::string> sourcePaths(projSettings->getSourcePaths());
	std::vector<std::string> includePaths(sourcePaths);

	std::vector<std::string> sourceExtensions = projSettings->getSourceExtensions();
	std::vector<std::string> includeExtensions = projSettings->getHeaderExtensions();

	if (sourcePaths.size())
	{
		m_fileManager = std::make_shared<FileManager>(sourcePaths, includePaths, sourceExtensions, includeExtensions);
	}
}

Parser::Arguments Project::getParserArguments() const
{
	std::shared_ptr<ProjectSettings> projSettings = ProjectSettings::getInstance();
	std::shared_ptr<ApplicationSettings> appSettings = ApplicationSettings::getInstance();

	Parser::Arguments args;

	if (!m_fileManager)
	{
		LOG_ERROR("No FileManger was created.");
		return args;
	}

	utility::append(args.compilerFlags, projSettings->getCompilerFlags());
	utility::append(args.compilerFlags, appSettings->getCompilerFlags());

	// Add the include paths as HeaderSearchPaths as well, so clang will also look here when searching include files.
	utility::append(args.systemHeaderSearchPaths, m_fileManager->getIncludePaths());
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
