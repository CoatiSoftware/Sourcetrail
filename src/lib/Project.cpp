#include "Project.h"

#include "component/view/DialogView.h"
#include "data/access/StorageAccessProxy.h"
#include "data/parser/TaskParseWrapper.h"
#include "data/parser/java/TaskParseJava.h"
#include "data/StorageProvider.h"
#include "data/PersistentStorage.h"
#include "data/TaskCleanStorage.h"
#include "data/TaskShowStatusDialog.h"
#include "data/TaskFinishParsing.h"
#include "data/TaskInjectStorage.h"
#include "settings/ApplicationSettings.h"
#include "settings/ProjectSettings.h"

#include "utility/file/FileRegister.h"
#include "utility/messaging/type/MessageClearErrorCount.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/scheduling/TaskDecoratorRepeat.h"
#include "utility/scheduling/TaskGroupSelector.h"
#include "utility/scheduling/TaskGroupSequence.h"
#include "utility/scheduling/TaskGroupParallel.h"
#include "utility/scheduling/TaskReturnSuccessWhile.h"
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

	bool needsFullRefresh = false;
	std::string question;

	switch (m_state)
	{
		case PROJECT_STATE_EMPTY:
			needsFullRefresh = true;
			break;

		case PROJECT_STATE_LOADED:
			break;

		case PROJECT_STATE_OUTDATED:
			question =
				"The project file was changed after the last indexing. The project needs to get fully reindexed to "
				"reflect the current project state. Do you want to reindex the project?";
			needsFullRefresh = true;
			break;

		case PROJECT_STATE_OUTVERSIONED:
			question =
				"This project was indexed with a different version of Coati. It needs to be fully reindexed to be used "
				"with this version of Coati. Do you want to reindex the project?";
			needsFullRefresh = true;
			break;

		case PROJECT_STATE_SETTINGS_UPDATED:
			question =
				"Some settings were changed, the project needs to be fully reindexed. "
				"Do you want to reindex the project?";
			needsFullRefresh = true;
			break;

		case PROJECT_STATE_NEEDS_MIGRATION:
			question =
				"This project was created with a different version of Coati. The project file needs to get updated and "
				"the project fully reindexed. Do you want to update the project file and reindex the project?";
			needsFullRefresh = true;

		default:
			break;
	}

	if (!forceRefresh && needsFullRefresh && question.size() && Application::getInstance()->hasGUI())
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

	if (requestIndex(forceRefresh, needsFullRefresh))
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
		m_storage->setMode(SqliteStorage::STORAGE_MODE_READ);
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

bool Project::requestIndex(bool forceRefresh, bool needsFullRefresh)
{
	if (!prepareIndexing())
	{
		return false;
	}

	const FileManager::FileSets fileSets = m_fileManager.fetchFilePaths(m_storage->getInfoOnAllFiles());

	std::set<FilePath> filesToClean;
	std::set<FilePath> filesToIndex;

	if (!needsFullRefresh)
	{
		utility::append(filesToClean, fileSets.removedFiles);
		utility::append(filesToClean, fileSets.updatedFiles);
		utility::append(filesToIndex, fileSets.addedFiles);
		utility::append(filesToIndex, fileSets.updatedFiles);

		// handle referencing paths
		const std::set<FilePath> referencingFilePaths = m_storage->getReferencing(utility::concat(
			fileSets.updatedFiles, fileSets.removedFiles
		));

		for (const FilePath& path : referencingFilePaths)
		{
			if (fileSets.removedFiles.find(path) == fileSets.removedFiles.end())
			{
				filesToClean.insert(path);
				filesToIndex.insert(path);
			}
		}

		// handle referenced paths
		std::set<FilePath> staticSourceFiles = fileSets.allFiles;
		for (const FilePath& path : fileSets.updatedFiles)
		{
			staticSourceFiles.erase(path);
		}
		const std::set<FilePath> staticReferencedFilePaths = m_storage->getReferenced(staticSourceFiles);
		const std::set<FilePath> dynamicReferencedFilePaths = m_storage->getReferenced(utility::concat(
			fileSets.updatedFiles, fileSets.removedFiles
		));

		for (const FilePath& path : dynamicReferencedFilePaths)
		{
			if (staticReferencedFilePaths.find(path) == staticReferencedFilePaths.end() &&
				staticSourceFiles.find(path) == staticSourceFiles.end())
			{
				// file may not be referenced anymore and will be reindexed if still needed
				filesToClean.insert(path);
			}
		}
	}

	bool fullRefresh = forceRefresh | needsFullRefresh;

	if (Application::getInstance()->hasGUI())
	{
		DialogView::IndexMode mode = m_dialogView->startIndexingDialog(
			filesToClean.size(), filesToIndex.size(), fileSets.allFiles.size(),
			forceRefresh, needsFullRefresh
		);

		switch (mode)
		{
			case DialogView::INDEX_ABORT:
				return false;
			case DialogView::INDEX_REFRESH:
				fullRefresh = false;
				break;
			case DialogView::INDEX_FULL:
				fullRefresh = true;
				break;
		}
	}

	if (fullRefresh)
	{
		filesToClean.clear();
		filesToIndex = fileSets.allFiles;
	}

	if (!filesToClean.size() && !filesToIndex.size())
	{
		MessageStatus("Nothing to refresh, all files are up-to-date.").dispatch();
		return false;
	}

	MessageStatus((fullRefresh ? "Reindexing Project" : "Refreshing Project"), false, true).dispatch();

	buildIndex(filesToClean, filesToIndex, fullRefresh);

	return true;
}

void Project::buildIndex(const std::set<FilePath>& filesToClean, const std::set<FilePath>& filesToIndex, bool fullRefresh)
{
	MessageClearErrorCount().dispatch();

	if (fullRefresh)
	{
		m_storage->clear();
	}

	m_storage->setProjectSettingsText(TextAccess::createFromFile(getProjectSettingsFilePath().str())->getText());

	std::shared_ptr<TaskGroupSequence> taskSequential = std::make_shared<TaskGroupSequence>();

	if (!filesToClean.empty())
	{
		taskSequential->addTask(std::make_shared<TaskCleanStorage>(
			m_storage.get(),
			utility::toVector(filesToClean),
			m_dialogView)
		);
	}

	const size_t indexerThreadCount = ApplicationSettings::getInstance()->getIndexerThreadCount();

	std::shared_ptr<FileRegister> fileRegister = std::make_shared<FileRegister>(&m_fileManager, indexerThreadCount > 1);

	if (!filesToIndex.empty())
	{
		fileRegister->setFilePaths(utility::toVector(filesToIndex));

		std::shared_ptr<TaskParseWrapper> taskParserWrapper = std::make_shared<TaskParseWrapper>(
			m_storage.get(),
			fileRegister,
			m_dialogView
		);
		taskSequential->addTask(taskParserWrapper);

		std::shared_ptr<TaskGroupParallel> taskParallelIndexing = std::make_shared<TaskGroupParallel>();
		taskParserWrapper->setTask(taskParallelIndexing);

		std::shared_ptr<StorageProvider> storageProvider = std::make_shared<StorageProvider>();

		for (size_t i = 0; i < indexerThreadCount && i < filesToIndex.size(); i++)
		{
			taskParallelIndexing->addChildTasks(
				std::make_shared<TaskDecoratorRepeat>(TaskDecoratorRepeat::CONDITION_WHILE_SUCCESS, Task::STATE_SUCCESS)->addChildTask(
					createIndexerTask(storageProvider, fileRegister)
				)
			);
		}

		taskParallelIndexing->addTask(
			std::make_shared<TaskGroupSequence>()->addChildTasks(
				std::make_shared<TaskDecoratorRepeat>(TaskDecoratorRepeat::CONDITION_WHILE_SUCCESS, Task::STATE_SUCCESS)->addChildTask(
					std::make_shared<TaskReturnSuccessWhile<int>>("indexer_count", TaskReturnSuccessWhile<int>::CONDITION_EQUALS, 0)
				),
				std::make_shared<TaskDecoratorRepeat>(TaskDecoratorRepeat::CONDITION_WHILE_SUCCESS, Task::STATE_SUCCESS)->addChildTask(
					std::make_shared<TaskGroupSequence>()->addChildTasks(
						// stopping when indexer count is zero, regardless wether there are still storages left to insert.
						std::make_shared<TaskReturnSuccessWhile<int>>("indexer_count", TaskReturnSuccessWhile<int>::CONDITION_GREATER_THAN, 0),
						std::make_shared<TaskGroupSelector>()->addChildTasks(
							std::make_shared<TaskInjectStorage>(storageProvider, m_storage),
							// continuing when indexer count is greater than zero, even if there are no storages right now.
							std::make_shared<TaskReturnSuccessWhile<int>>("indexer_count", TaskReturnSuccessWhile<int>::CONDITION_GREATER_THAN, 0)
						)
					)
				)
			)
		);

		taskSequential->addTask( // we don't need to hide this dialog again, because it's overridden by other dialogs later on.
			std::make_shared<TaskShowStatusDialog>("Finish Indexing", "Saving\nRemaining Data", m_dialogView)
		);

		taskSequential->addTask(
			std::make_shared<TaskDecoratorRepeat>(TaskDecoratorRepeat::CONDITION_WHILE_SUCCESS, Task::STATE_SUCCESS)->addChildTask(
				std::make_shared<TaskInjectStorage>(storageProvider, m_storage)
			)
		);
	}

	taskSequential->addTask(std::make_shared<TaskFinishParsing>(m_storage.get(), m_storageAccessProxy, fileRegister, m_dialogView));

	Task::dispatch(taskSequential);
}

bool Project::prepareIndexing()
{
	return true;
}

bool Project::prepareRefresh()
{
	return true;
}
