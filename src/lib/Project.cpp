#include "Project.h"

#include "component/view/DialogView.h"
#include "data/access/StorageAccessProxy.h"
#include "data/parser/cxx/TaskParseWrapper.h"
#include "data/parser/java/TaskParseJava.h"
#include "data/PersistentStorage.h"
#include "data/TaskCleanStorage.h"
#include "settings/ApplicationSettings.h"
#include "settings/ProjectSettings.h"

#include "utility/file/FileRegister.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/scheduling/TaskGroupSequential.h"
#include "utility/scheduling/TaskGroupParallel.h"
#include "utility/text/TextAccess.h"
#include "utility/utility.h"
#include "utility/utilityString.h"
#include "utility/Version.h"

#include "Application.h"
#include "CxxProject.h"
#include "JavaProject.h"
#include "isTrial.h"

std::shared_ptr<Project> Project::create(
	const FilePath& projectSettingsFile, StorageAccessProxy* storageAccessProxy, DialogView* dialogView)
{
	std::shared_ptr<Project> project;

	switch (ProjectSettings::getLanguageOfProject(projectSettingsFile))
	{
	case LANGUAGE_C:
	case LANGUAGE_CPP:
		{
			project = std::shared_ptr<CxxProject>(new CxxProject(
				std::make_shared<CxxProjectSettings>(projectSettingsFile), storageAccessProxy, dialogView
			));
		}
		break;
	case LANGUAGE_JAVA:
		{
			project = std::shared_ptr<JavaProject>(new JavaProject(
				std::make_shared<JavaProjectSettings>(projectSettingsFile), storageAccessProxy, dialogView
			));
		}
		break;
	default:
		break;
	}

	if (project)
	{
		project->load();
	}
	return project;
}

Project::~Project()
{
}

bool Project::refresh(bool forceRefresh)
{
	if (m_state == PROJECT_STATE_NOT_LOADED)
	{
		return false;
	}

	std::string question;

	if (!forceRefresh)
	{
		switch (m_state)
		{
			case PROJECT_STATE_EMPTY:
				forceRefresh = true;
				break;

			case PROJECT_STATE_LOADED:
				break;

			case PROJECT_STATE_OUTDATED:
				question =
					"The project file was changed after the last indexing. The project needs to get fully reindexed to "
					"reflect the current project state. Do you want to reindex the project?";
				forceRefresh = true;
				break;

			case PROJECT_STATE_OUTVERSIONED:
				question =
					"This project was indexed with a different version of Coati. It needs to be fully reindexed to be used "
					"with this version of Coati. Do you want to reindex the project?";
				forceRefresh = true;
				break;

			case PROJECT_STATE_SETTINGS_UPDATED:
				question =
					"Some settings were changed, the project needs to be fully reindexed. "
					"Do you want to reindex the project?";
				forceRefresh = true;
				break;

			default:
				break;
		}
	}

	if (forceRefresh && question.size() && Application::getInstance()->hasGUI() && !isTrial())
	{
		std::vector<std::string> options;
		options.push_back("Yes");
		options.push_back("No");
		int result = m_dialogView->confirm(question, options);

		if (result == 1)
		{
			return false;
		}
	}

	if (!prepareRefresh())
	{
		return false;
	}

	getProjectSettings()->reload();

	updateFileManager(m_fileManager);

	if (buildIndex(forceRefresh))
	{
		m_storageAccessProxy->setSubject(m_storage.get());

		m_state = PROJECT_STATE_LOADED;

		return true;
	}

	return false;
}

FilePath Project::getProjectSettingsFilePath() const
{
	return getProjectSettings()->getFilePath();
}

LanguageType Project::getLanguage() const
{
	return getProjectSettings()->getLanguage();
}

std::string Project::getDescription() const
{
	return getProjectSettings()->getDescription();
}

bool Project::settingsEqualExceptNameAndLocation(const ProjectSettings& otherSettings) const
{
	return getProjectSettings()->equalsExceptNameAndLocation(otherSettings);
}

void Project::setStateSettingsUpdated()
{
	if (m_state != PROJECT_STATE_NOT_LOADED && m_state != PROJECT_STATE_EMPTY)
	{
		m_state = PROJECT_STATE_SETTINGS_UPDATED;
	}
}

void Project::logStats() const
{
	m_storage->logStats();
}

Project::Project(StorageAccessProxy* storageAccessProxy, DialogView* dialogView)
	: m_storageAccessProxy(storageAccessProxy)
	, m_dialogView(dialogView)
	, m_state(PROJECT_STATE_NOT_LOADED)
{
}

DialogView* Project::getDialogView() const
{
	return m_dialogView;
}

void Project::load()
{
	m_storageAccessProxy->setSubject(nullptr);

	const std::shared_ptr<ProjectSettings> projectSettings = getProjectSettings();
	bool loadedSettings = projectSettings->reload();

	if (!loadedSettings)
	{
		return;
	}

	NameHierarchy::setDelimiter(getSymbolNameDelimiterForLanguage(projectSettings->getLanguage()));

	const FilePath projectSettingsPath = projectSettings->getFilePath();
	const FilePath dbPath = FilePath(projectSettingsPath).replaceExtension("coatidb");

	m_storage = std::make_shared<PersistentStorage>(dbPath);

	if (m_storage->isEmpty())
	{
		m_state = PROJECT_STATE_EMPTY;
		m_storage->setup();
	}
	else if (m_storage->isIncompatible())
	{
		m_state = PROJECT_STATE_OUTVERSIONED;
	}
	else if (TextAccess::createFromFile(projectSettingsPath.str())->getText() != m_storage->getProjectSettingsText())
	{
		m_state = PROJECT_STATE_OUTDATED;
	}
	else
	{
		m_state = PROJECT_STATE_LOADED;
	}

	if (m_state == PROJECT_STATE_LOADED || m_state == PROJECT_STATE_OUTDATED)
	{
		m_storage->finishParsing();
		m_storageAccessProxy->setSubject(m_storage.get());

		MessageFinishedParsing().dispatch();
		MessageStatus("Finished Loading", false, false).dispatch();
	}
	else
	{
		MessageStatus("Project not loaded", false, false).dispatch();
	}

	if (m_state != PROJECT_STATE_LOADED)
	{
		MessageRefresh().dispatch();
	}
}

bool Project::buildIndex(bool forceRefresh)
{
	if (!prepareIndexing())
	{
		return false;
	}

	m_fileManager.fetchFilePaths(
		forceRefresh ? std::vector<FileInfo>() : m_storage->getInfoOnAllFiles()
	);

	std::set<FilePath> addedFilePaths = m_fileManager.getAddedFilePaths();
	std::set<FilePath> updatedFilePaths = m_fileManager.getUpdatedFilePaths();
	std::set<FilePath> removedFilePaths = m_fileManager.getRemovedFilePaths();

	if (!forceRefresh)
	{
		utility::append(updatedFilePaths, m_storage->getDependingFilePaths(updatedFilePaths));
		utility::append(updatedFilePaths, m_storage->getDependingFilePaths(removedFilePaths));
	}

	std::vector<FilePath> filesToClean;
	filesToClean.insert(filesToClean.end(), removedFilePaths.begin(), removedFilePaths.end());
	filesToClean.insert(filesToClean.end(), updatedFilePaths.begin(), updatedFilePaths.end());

	std::vector<FilePath> filesToParse;
	filesToParse.insert(filesToParse.end(), addedFilePaths.begin(), addedFilePaths.end());
	filesToParse.insert(filesToParse.end(), updatedFilePaths.begin(), updatedFilePaths.end());


	if (!filesToClean.size() && !filesToParse.size())
	{
		MessageStatus("Nothing to refresh, all files are up-to-date.").dispatch();
		return false;
	}

	if (Application::getInstance()->hasGUI())
	{
		bool doIndex = m_dialogView->startIndexingDialog(filesToClean.size(), filesToParse.size());

		if (!doIndex)
		{
			return false;
		}
	}

	if (forceRefresh)
	{
		m_storage->clear();
	}

	m_storage->setProjectSettingsText(TextAccess::createFromFile(getProjectSettingsFilePath().str())->getText());

	std::shared_ptr<TaskGroupSequential> taskSequential = std::make_shared<TaskGroupSequential>();

	if (filesToClean.size())
	{
		taskSequential->addTask(std::make_shared<TaskCleanStorage>(m_storage.get(), filesToClean, m_dialogView));
	}

	int indexerThreadCount = ApplicationSettings::getInstance()->getIndexerThreadCount();

	std::shared_ptr<FileRegister> fileRegister = std::make_shared<FileRegister>(&m_fileManager, indexerThreadCount > 1);
	fileRegister->setFilePaths(filesToParse);

	std::shared_ptr<TaskParseWrapper> taskParserWrapper = std::make_shared<TaskParseWrapper>(
		m_storage.get(),
		fileRegister,
		m_dialogView
	);
	taskSequential->addTask(taskParserWrapper);

	std::shared_ptr<TaskGroupParallel> taskParallelIndexing = std::make_shared<TaskGroupParallel>();
	taskParserWrapper->setTask(taskParallelIndexing);

	std::shared_ptr<std::mutex> storageMutex = std::make_shared<std::mutex>();

	for (int i = 0; i < indexerThreadCount; i++)
	{
		taskParallelIndexing->addTask(createIndexerTask(m_storage.get(), storageMutex, fileRegister));
	}

	Task::dispatch(taskSequential);

	return true;
}

bool Project::prepareIndexing()
{
	return true;
}

bool Project::prepareRefresh()
{
	return true;
}
