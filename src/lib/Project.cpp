#include "Project.h"

#include "data/access/StorageAccessProxy.h"
#include "data/graph/Token.h"
#include "data/parser/cxx/TaskParseCxx.h"
#include "data/parser/cxx/TaskParseWrapper.h"
#include "data/PersistentStorage.h"
#include "data/TaskCleanStorage.h"
#include "settings/ApplicationSettings.h"
#include "settings/ProjectSettings.h"

#include "utility/file/FileRegister.h"
#include "utility/file/FileSystem.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/scheduling/TaskGroupSequential.h"
#include "utility/scheduling/TaskGroupParallel.h"
#include "utility/utility.h"
#include "utility/Version.h"

std::shared_ptr<Project> Project::create(StorageAccessProxy* storageAccessProxy)
{
	std::shared_ptr<Project> ptr(new Project(storageAccessProxy));
	return ptr;
}

Project::~Project()
{
}

const FilePath& Project::getProjectSettingsFilePath() const
{
	return m_projectSettingsFilepath;
}

Project::ProjectState Project::load(const FilePath& projectSettingsFile)
{
	m_state = PROJECT_NONE;

	bool success = true;
	if (!projectSettingsFile.empty())
	{
		success = ProjectSettings::getInstance()->load(projectSettingsFile);
	}

	if (success)
	{
		setProjectSettingsFilePath(projectSettingsFile);
		updateFileManager();

		switch (m_state)
		{
			case PROJECT_NONE:
				break;

			case PROJECT_EMPTY:
				parseCode();
				break;

			case PROJECT_LOADED:
			case PROJECT_OUTDATED:
				m_storage->finishParsing();
				MessageFinishedParsing(0, 0, 0, true).dispatch();
				break;

			case PROJECT_OUTVERSIONED:
				m_storage.reset();
				break;
		}
	}

	return m_state;
}

Project::ProjectState Project::reload()
{
	if (m_state == PROJECT_LOADED &&
		FileSystem::getFileInfoForPath(m_projectSettingsFilepath).lastWriteTime >
		FileSystem::getFileInfoForPath(m_storage->getDbFilePath()).lastWriteTime)
	{
		m_state = PROJECT_OUTDATED;
	}
	else if (!m_projectSettingsFilepath.empty() && (m_state == PROJECT_EMPTY || m_state == PROJECT_LOADED))
	{
		ProjectSettings::getInstance()->load(m_projectSettingsFilepath);
		updateFileManager();

		parseCode();
	}

	return m_state;
}

void Project::clearStorage()
{
	if (m_state == PROJECT_OUTVERSIONED)
	{
		loadStorage(m_projectSettingsFilepath);
	}

	if (m_storage)
	{
		m_storage->clear();
		m_state = PROJECT_EMPTY;
	}
}

void Project::parseCode()
{
	if (m_projectSettingsFilepath.empty())
	{
		return;
	}

	m_fileManager.fetchFilePaths(m_storage->getInfoOnAllFiles());
	std::set<FilePath> addedFilePaths = m_fileManager.getAddedFilePaths();
	std::set<FilePath> updatedFilePaths = m_fileManager.getUpdatedFilePaths();
	std::set<FilePath> removedFilePaths = m_fileManager.getRemovedFilePaths();

	utility::append(updatedFilePaths, m_storage->getDependingFilePaths(updatedFilePaths));
	utility::append(updatedFilePaths, m_storage->getDependingFilePaths(removedFilePaths));

	std::vector<FilePath> filesToClean;
	filesToClean.insert(filesToClean.end(), removedFilePaths.begin(), removedFilePaths.end());
	filesToClean.insert(filesToClean.end(), updatedFilePaths.begin(), updatedFilePaths.end());

	std::vector<FilePath> filesToParse;
	filesToParse.insert(filesToParse.end(), addedFilePaths.begin(), addedFilePaths.end());
	filesToParse.insert(filesToParse.end(), updatedFilePaths.begin(), updatedFilePaths.end());

	m_state = PROJECT_LOADED;

	if (!filesToClean.size() && !filesToParse.size())
	{
		MessageFinishedParsing(0, 0, 0, true).dispatch();
		return;
	}

	std::shared_ptr<TaskGroupSequential> taskSequential = std::make_shared<TaskGroupSequential>();
	taskSequential->addTask(std::make_shared<TaskCleanStorage>(m_storage.get(), filesToClean));

	int indexerThreadCount = ApplicationSettings::getInstance()->getIndexerThreadCount();

	std::shared_ptr<FileRegister> fileRegister = std::make_shared<FileRegister>(&m_fileManager, indexerThreadCount > 1);
	fileRegister->setFilePaths(filesToParse);

	std::shared_ptr<TaskGroupParallel> taskParallel = std::make_shared<TaskGroupParallel>();

	taskSequential->addTask(std::make_shared<TaskParseWrapper>(
		taskParallel,
		m_storage.get(),
		fileRegister
	));

	std::shared_ptr<std::mutex> storageMutex = std::make_shared<std::mutex>();

	for (int i = 0; i < indexerThreadCount; i++)
	{
		taskParallel->addTask(std::make_shared<TaskParseCxx>(
			m_storage.get(),
			storageMutex,
			fileRegister,
			getParserArguments()
		));
	}

	Task::dispatch(taskSequential);
}

void Project::logStats() const
{
	m_storage->logStats();
}

void Project::setProjectSettingsFilePath(const FilePath& path)
{
	if (path.empty())
	{
		m_storage.reset();
		m_state = PROJECT_NONE;
	}
	else
	{
		loadStorage(path);

		Version version = m_storage->getVersion();
		if (version.isEmpty())
		{
			m_state = PROJECT_EMPTY;
			m_storage->init();
		}
		else if (version.isDifferentStorageVersionThan(Version::getApplicationVersion()))
		{
			m_state = PROJECT_OUTVERSIONED;
			m_storage.reset();
		}
		else if (FileSystem::getFileInfoForPath(path).lastWriteTime > FileSystem::getFileInfoForPath(m_storage->getDbFilePath()).lastWriteTime)
		{
			m_state = PROJECT_OUTDATED;
		}
		else
		{
			m_state = PROJECT_LOADED;
		}
	}

	m_storageAccessProxy->setSubject(m_storage.get());
	m_projectSettingsFilepath = path;
}

void Project::loadStorage(const FilePath& path)
{
	FilePath dbPath = FilePath(path).replaceExtension("coatidb");
	if (!m_storage || path != m_projectSettingsFilepath || !dbPath.exists())
	{
		m_storage = std::make_shared<PersistentStorage>(dbPath);
	}
}

void Project::updateFileManager()
{
	std::shared_ptr<ProjectSettings> projSettings = ProjectSettings::getInstance();

	std::vector<FilePath> sourcePaths = projSettings->getAbsoluteSourcePaths();
	std::vector<FilePath> headerPaths = sourcePaths;

	std::vector<std::string> sourceExtensions;

	if (projSettings->getCompilationDatabasePath().exists())
	{
		sourcePaths = TaskParseCxx::getSourceFilesFromCDB(projSettings->getCompilationDatabasePath());
	}
	else
	{
		sourceExtensions = projSettings->getSourceExtensions();
	}

	std::vector<FilePath> excludePaths = projSettings->getAbsoluteExcludePaths();

	m_fileManager.setPaths(sourcePaths, headerPaths, excludePaths, sourceExtensions);
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

	utility::append(args.systemHeaderSearchPaths, projSettings->getAbsoluteHeaderSearchPaths());

	utility::append(args.systemHeaderSearchPaths, appSettings->getHeaderSearchPathsExpanded());

	// Add all subdirectories of the header search paths
	if (projSettings->getUseSourcePathsForHeaderSearch())
	{
		std::vector<FilePath> headerSearchSubPaths;
		for (FilePath p : projSettings->getSourcePaths())
		{
			utility::append(headerSearchSubPaths, FileSystem::getSubDirectories(p));
		}

		utility::append(args.systemHeaderSearchPaths, utility::unique(headerSearchSubPaths));
	}

	utility::append(args.frameworkSearchPaths, projSettings->getAbsoluteFrameworkSearchPaths());
	utility::append(args.frameworkSearchPaths, appSettings->getFrameworkSearchPathsExpanded());

	args.language = projSettings->getLanguage();
	args.languageStandard = projSettings->getStandard();
	args.compilationDatabasePath = projSettings->getCompilationDatabasePath();

	return args;
}

Project::Project(StorageAccessProxy* storageAccessProxy)
	: m_storageAccessProxy(storageAccessProxy)
	, m_state(PROJECT_NONE)
{
}
