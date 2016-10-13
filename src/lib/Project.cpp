#include "Project.h"

#include "component/view/DialogView.h"
#include "data/access/StorageAccessProxy.h"
#include "data/parser/TaskParseWrapper.h"
#include "data/parser/java/TaskParseJava.h"
#include "data/StorageProvider.h"
#include "data/PersistentStorage.h"
#include "data/TaskCleanStorage.h"
#include "data/TaskFinishParsing.h"
#include "data/TaskInjectStorage.h"
#include "settings/ApplicationSettings.h"
#include "settings/ProjectSettings.h"

#include "utility/file/FileRegister.h"
#include "utility/messaging/type/MessageClearErrorCount.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/scheduling/TaskGroupSequential.h"
#include "utility/scheduling/TaskGroupParallel.h"
#include "utility/scheduling/TaskRepeatWhileSuccess.h"
#include "utility/text/TextAccess.h"
#include "utility/utility.h"
#include "utility/utilityString.h"
#include "utility/Version.h"

#include "Application.h"
#include "CxxProject.h"
#include "JavaProject.h"

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

			case PROJECT_STATE_NEEDS_MIGRATION:
				question =
					"This project was created with a different version of Coati. The project file needs to get updated and "
					"the project fully reindexed. Do you want to update the project file and reindex the project?";
				forceRefresh = true;

			default:
				break;
		}
	}

	if (forceRefresh && question.size() && Application::getInstance()->hasGUI())
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

	if (m_state == PROJECT_STATE_NEEDS_MIGRATION)
	{
		getProjectSettings()->migrate();
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

const std::vector<FilePath>& Project::getSourcePaths() const
{
	return m_fileManager.getSourcePaths();
}

void Project::load()
{
	m_storageAccessProxy->setSubject(nullptr);

	std::shared_ptr<ProjectSettings> projectSettings = getProjectSettings();
	bool loadedSettings = projectSettings->reload();

	if (!loadedSettings)
	{
		return;
	}

	NameHierarchy::setDelimiter(getSymbolNameDelimiterForLanguage(projectSettings->getLanguage()));

	const FilePath projectSettingsPath = projectSettings->getFilePath();
	const FilePath dbPath = FilePath(projectSettingsPath).replaceExtension("coatidb");

	m_storage = std::make_shared<PersistentStorage>(dbPath);

	bool canLoad = false;

	if (projectSettings->needMigration())
	{
		m_state = PROJECT_STATE_NEEDS_MIGRATION;

		if (!m_storage->isEmpty() && !m_storage->isIncompatible())
		{
			canLoad = true;
		}
	}
	else if (m_storage->isEmpty())
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
		canLoad = true;
	}
	else
	{
		m_state = PROJECT_STATE_LOADED;
		canLoad = true;
	}

	if (canLoad)
	{
		m_storage->buildCaches();
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

	std::set<FilePath> filesToClean;
	std::set<FilePath> filesToParse;

	if (!forceRefresh)
	{
		std::set<FilePath> dependingFilePaths;
		utility::append(dependingFilePaths, m_storage->getDependingFilePaths(updatedFilePaths));
		utility::append(dependingFilePaths, m_storage->getDependingFilePaths(removedFilePaths));

		for (const FilePath& path : dependingFilePaths)
		{
			if (removedFilePaths.find(path) == removedFilePaths.end())
			{
				updatedFilePaths.insert(path);
			}
		}

		utility::append(filesToClean, dependingFilePaths);
	}

	utility::append(filesToClean, removedFilePaths);
	utility::append(filesToClean, updatedFilePaths);

	utility::append(filesToParse, addedFilePaths);
	utility::append(filesToParse, updatedFilePaths);

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

	MessageClearErrorCount().dispatch();

	if (forceRefresh)
	{
		m_storage->clear();
	}

	m_storage->setProjectSettingsText(TextAccess::createFromFile(getProjectSettingsFilePath().str())->getText());

	std::shared_ptr<TaskGroupSequential> taskSequential = std::make_shared<TaskGroupSequential>();

	if (filesToClean.size())
	{
		taskSequential->addTask(std::make_shared<TaskCleanStorage>(
			m_storage.get(),
			utility::toVector(filesToClean),
			m_dialogView)
		);
	}

	const int indexerThreadCount = ApplicationSettings::getInstance()->getIndexerThreadCount();

	std::shared_ptr<FileRegister> fileRegister = std::make_shared<FileRegister>(&m_fileManager, indexerThreadCount > 1);

	if (filesToParse.size())
	{
		fileRegister->setFilePaths(utility::toVector(filesToParse));

		std::shared_ptr<TaskParseWrapper> taskParserWrapper = std::make_shared<TaskParseWrapper>(
			fileRegister,
			m_dialogView
		);
		taskSequential->addTask(taskParserWrapper);

		std::shared_ptr<TaskGroupParallel> taskParallelIndexing = std::make_shared<TaskGroupParallel>();
		taskParserWrapper->setTask(taskParallelIndexing);

		std::shared_ptr<StorageProvider> storageProvider = std::make_shared<StorageProvider>();

		for (int i = 0; i < indexerThreadCount; i++)
		{
			std::shared_ptr<TaskRepeatWhileSuccess> taskRepeat = std::make_shared<TaskRepeatWhileSuccess>(Task::STATE_SUCCESS);
			taskParallelIndexing->addTask(taskRepeat);
			taskRepeat->setTask(createIndexerTask(storageProvider, fileRegister));
		}

		std::shared_ptr<TaskRepeatWhileSuccess> taskRepeat = std::make_shared<TaskRepeatWhileSuccess>(Task::STATE_SUCCESS);
		taskParallelIndexing->addTask(taskRepeat);
		taskRepeat->setTask(std::make_shared<TaskInjectStorage>(storageProvider, m_storage));
	}

	taskSequential->addTask(std::make_shared<TaskFinishParsing>(m_storage.get(), m_storageAccessProxy, fileRegister, m_dialogView));

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
