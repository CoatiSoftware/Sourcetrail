#include "project/Project.h"

#include "Application.h"
#include "component/view/DialogView.h"
#include "data/access/StorageAccessProxy.h"
#include "data/indexer/IndexerCommand.h"
#include "data/indexer/IndexerCommandList.h"
#include "data/indexer/TaskBuildIndex.h"
#include "data/parser/TaskParseWrapper.h"
#include "data/storage/StorageProvider.h"
#include "data/storage/PersistentStorage.h"
#include "data/TaskCleanStorage.h"
#include "data/TaskMergeStorages.h"
#include "data/TaskShowStatusDialog.h"
#include "data/TaskFinishParsing.h"
#include "data/TaskInjectStorage.h"
#include "project/SourceGroup.h"
#include "project/SourceGroupFactory.h"
#include "settings/ApplicationSettings.h"
#include "settings/ProjectSettings.h"

#include "utility/file/FilePath.h"
#include "utility/file/FileSystem.h"
#include "utility/messaging/type/MessageClearErrorCount.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/scheduling/TaskDecoratorRepeat.h"
#include "utility/scheduling/TaskGroupSelector.h"
#include "utility/scheduling/TaskGroupSequence.h"
#include "utility/scheduling/TaskGroupParallel.h"
#include "utility/scheduling/TaskReturnSuccessWhile.h"
#include "utility/scheduling/TaskSetValue.h"
#include "utility/ScopedFunctor.h"
#include "utility/text/TextAccess.h"
#include "utility/utility.h"
#include "utility/utilityApp.h"
#include "utility/utilityString.h"

Project::Project(std::shared_ptr<ProjectSettings> settings, StorageAccessProxy* storageAccessProxy)
	: m_settings(settings)
	, m_storageAccessProxy(storageAccessProxy)
	, m_state(PROJECT_STATE_NOT_LOADED)
{
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
				"This project was indexed with a different version of Sourcetrail. It needs to be fully reindexed to be used "
				"with this version of Sourcetrail. Do you want to reindex the project?";
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
				"This project was created with a different version of Sourcetrail. The project file needs to get updated and "
				"the project fully reindexed. Do you want to update the project file and reindex the project?";
			needsFullRefresh = true;

		default:
			break;
	}

	std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView();

	if (
		ApplicationSettings::getInstance()->getLoggingEnabled() &&
		ApplicationSettings::getInstance()->getVerboseIndexerLoggingEnabled() &&
		Application::getInstance()->hasGUI()
		)
	{
		std::vector<std::string> options = { "Yes", "No" };
		int result = dialogView->confirm(
			"Warning: You are about to index your project with the \"verbose indexer logging\" setting "
			"enabled. This will cause a significant slowdown in indexing performance. Do you want to proceed?",
			options
		);

		if (result == 1)
		{
			return false;
		}
	}

	if (!forceRefresh && needsFullRefresh && question.size() && Application::getInstance()->hasGUI())
	{
		std::vector<std::string> options = { "Yes", "No"};
		int result = dialogView->confirm(question, options);

		if (result == 1)
		{
			return false;
		}
	}

	dialogView->showUnknownProgressDialog("Preparing Project", "Processing Files");

	ScopedFunctor dialogHider([&dialogView](){
		dialogView->hideUnknownProgressDialog();
	});


	if (m_state == PROJECT_STATE_NEEDS_MIGRATION)
	{
		m_settings->migrate();
	}

	m_settings->reload();

	m_sourceGroups = SourceGroupFactory::getInstance()->createSourceGroups(m_settings->getAllSourceGroupSettings());
	for (const std::shared_ptr<SourceGroup>& sourceGroup: m_sourceGroups)
	{
		if (!sourceGroup->prepareRefresh())
		{
			return false;
		}
	}

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
	return m_settings->getFilePath();
}

std::string Project::getDescription() const
{
	return m_settings->getDescription();
}

bool Project::settingsEqualExceptNameAndLocation(const ProjectSettings& otherSettings) const
{
	return m_settings->equalsExceptNameAndLocation(otherSettings);
}

void Project::setStateSettingsUpdated()
{
	if (m_state != PROJECT_STATE_NOT_LOADED && m_state != PROJECT_STATE_EMPTY)
	{
		m_state = PROJECT_STATE_SETTINGS_UPDATED;
	}
}

void Project::load()
{
	m_storageAccessProxy->setSubject(nullptr);

	bool loadedSettings = m_settings->reload();

	if (!loadedSettings)
	{
		return;
	}

	const FilePath projectSettingsPath = m_settings->getFilePath();

	const std::string dbExtension = (projectSettingsPath.extension() == ".coatiproject" ? "coatidb" : "srctrldb");
	const FilePath dbPath = FilePath(projectSettingsPath).replaceExtension(dbExtension);
	const FilePath bookmarkPath = FilePath(projectSettingsPath).replaceExtension("srctrlbm");

	m_storage = std::make_shared<PersistentStorage>(dbPath, bookmarkPath);

	bool canLoad = false;

	if (m_settings->needMigration())
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
	}
	else if (m_storage->isIncompatible())
	{
		m_state = PROJECT_STATE_OUTVERSIONED;
	}
	else if (utility::replace(TextAccess::createFromFile(projectSettingsPath)->getText(), "\r", "") !=
		utility::replace(TextAccess::createFromString(m_storage->getProjectSettingsText())->getText(), "\r", ""))
	{
		m_state = PROJECT_STATE_OUTDATED;
		canLoad = true;
	}
	else
	{
		m_state = PROJECT_STATE_LOADED;
		canLoad = true;
	}

	m_storage->setup();

	m_sourceGroups = SourceGroupFactory::getInstance()->createSourceGroups(m_settings->getAllSourceGroupSettings());

	if (canLoad)
	{
		m_storage->setMode(SqliteStorage::STORAGE_MODE_READ);
		m_storage->buildCaches();
		m_storageAccessProxy->setSubject(m_storage.get());

		if (Application::getInstance()->hasGUI())
		{
			MessageFinishedParsing().dispatch();
		}
		MessageStatus("Finished Loading", false, false).dispatch();
	}
	else
	{
		MessageStatus("Project not loaded", false, false).dispatch();
	}

	if (m_state != PROJECT_STATE_LOADED && Application::getInstance()->hasGUI())
	{
		MessageRefresh().dispatch();
	}
}

bool Project::requestIndex(bool forceRefresh, bool needsFullRefresh)
{
	std::set<FilePath> allSourceFilePaths;
	for (const std::shared_ptr<SourceGroup>& sourceGroup: m_sourceGroups)
	{
		if (!sourceGroup->prepareIndexing())
		{
			return false;
		}
		sourceGroup->fetchAllSourceFilePaths();
		utility::append(allSourceFilePaths, sourceGroup->getAllSourceFilePaths());
	}

	std::set<FilePath> filesToClean;
	std::set<FilePath> filesToAdd;
	if (!needsFullRefresh)
	{
		std::set<FilePath> unchangedFilePaths;
		std::set<FilePath> changedFilePaths;
		for (const FileInfo& info: m_storage->getInfoOnAllFiles())
		{
			if (info.path.exists())
			{
				if (didFileChange(info))
				{
					changedFilePaths.insert(info.path);
				}
				else
				{
					unchangedFilePaths.insert(info.path);
				}
			}
			else
			{
				// file has been removed
				changedFilePaths.insert(info.path);
			}
		}

		filesToClean = changedFilePaths;

		// handle referencing paths
		utility::append(filesToClean, m_storage->getReferencing(changedFilePaths));

		// handle referenced paths
		std::set<FilePath> staticSourceFiles = allSourceFilePaths;
		for (const FilePath& path: changedFilePaths)
		{
			staticSourceFiles.erase(path);
		}

		const std::set<FilePath> staticReferencedFilePaths = m_storage->getReferenced(staticSourceFiles);
		const std::set<FilePath> dynamicReferencedFilePaths = m_storage->getReferenced(changedFilePaths);

		for (const FilePath& path : dynamicReferencedFilePaths)
		{
			if (staticReferencedFilePaths.find(path) == staticReferencedFilePaths.end() &&
				staticSourceFiles.find(path) == staticSourceFiles.end())
			{
				// file may not be referenced anymore and will be reindexed if still needed
				filesToClean.insert(path);
			}
		}

		for (const FilePath& path: unchangedFilePaths)
		{
			staticSourceFiles.erase(path);
		}
		filesToAdd = staticSourceFiles;
	}


	std::set<FilePath> staticSourceFilePaths;
	for (const FilePath& path: allSourceFilePaths)
	{
		if (filesToClean.find(path) == filesToClean.end() && filesToAdd.find(path) == filesToAdd.end())
		{
			staticSourceFilePaths.insert(path);
		}
	}

	std::set<FilePath> filesToIndex;
	for (const std::shared_ptr<SourceGroup>& sourceGroup: m_sourceGroups)
	{
		sourceGroup->fetchSourceFilePathsToIndex(staticSourceFilePaths);
		utility::append(filesToIndex, sourceGroup->getSourceFilePathsToIndex());
	}

	bool fullRefresh = forceRefresh | needsFullRefresh;

	if (Application::getInstance()->hasGUI())
	{
		DialogView::IndexingOptions options;
		options.fullRefreshVisible = !needsFullRefresh;
		options.fullRefresh = forceRefresh;

		Application::getInstance()->getDialogView()->hideUnknownProgressDialog();

		options = Application::getInstance()->getDialogView()->startIndexingDialog(
			filesToClean.size(), filesToIndex.size(), allSourceFilePaths.size(), options);

		if (!options.startIndexing)
		{
			return false;
		}

		fullRefresh = options.fullRefresh | needsFullRefresh;
	}

	if (fullRefresh)
	{
		filesToClean.clear();
		filesToIndex = allSourceFilePaths;
	}

	if (!filesToClean.size() && !filesToIndex.size())
	{
		if (!Application::getInstance()->hasGUI())
		{
			MessageFinishedParsing().dispatch();
		}

		MessageStatus("Nothing to refresh, all files are up-to-date.").dispatch();
		return false;
	}

	MessageStatus((fullRefresh ? "Reindexing Project" : "Refreshing Project"), false, true).dispatch();

	buildIndex(filesToIndex, filesToClean, fullRefresh);

	return true;
}

void Project::buildIndex(
	const std::set<FilePath>& filesToIndex, const std::set<FilePath>& filesToClean, bool fullRefresh)
{
	MessageClearErrorCount().dispatch();
	if (fullRefresh)
	{
		m_storage->clear();
	}

	m_storage->setProjectSettingsText(TextAccess::createFromFile(getProjectSettingsFilePath())->getText());

	std::shared_ptr<TaskGroupSequence> taskSequential = std::make_shared<TaskGroupSequence>();

	// add task for cleaning the database
	if (!filesToClean.empty())
	{
		taskSequential->addTask(std::make_shared<TaskCleanStorage>(
			m_storage.get(),
			utility::toVector(filesToClean)
		));
	}

	std::shared_ptr<IndexerCommandList> indexerCommandList = std::make_shared<IndexerCommandList>();
	for (const std::shared_ptr<SourceGroup>& sourceGroup : m_sourceGroups)
	{
		for (const std::shared_ptr<IndexerCommand>& command : sourceGroup->getIndexerCommands(filesToIndex, fullRefresh))
		{
			indexerCommandList->addCommand(command);
		}
	}

	if (indexerCommandList->size() > 0)
	{
		int indexerThreadCount = ApplicationSettings::getInstance()->getIndexerThreadCount();
		if (indexerThreadCount <= 0)
		{
			indexerThreadCount = utility::getIdealThreadCount();
			if (indexerThreadCount <= 0)
			{
				indexerThreadCount = 4; // setting to some fallback value
			}
		}

		indexerThreadCount = std::min<int>(indexerThreadCount, indexerCommandList->size());
		if (indexerThreadCount > 1)
		{
			indexerCommandList->shuffle();
		}

		std::shared_ptr<StorageProvider> storageProvider = std::make_shared<StorageProvider>();
		// add tasks for setting some variables on the blackboard that are used during indexing
		taskSequential->addTask(std::make_shared<TaskSetValue<int>>("source_file_count", indexerCommandList->size()));
		taskSequential->addTask(std::make_shared<TaskSetValue<int>>("indexed_source_file_count", 0));
		taskSequential->addTask(std::make_shared<TaskSetValue<int>>("indexer_count", 0));

		std::shared_ptr<TaskParseWrapper> taskParserWrapper = std::make_shared<TaskParseWrapper>(m_storage.get());

		taskSequential->addTask(taskParserWrapper);
		std::shared_ptr<TaskGroupParallel> taskParallelIndexing = std::make_shared<TaskGroupParallel>();
		taskParserWrapper->setTask(taskParallelIndexing);
		// add task for indexing
		if (indexerThreadCount > 0)
		{
			bool multiProcess = ApplicationSettings::getInstance()->getMultiProcessIndexingEnabled() && hasCxxSourceGroup();

			taskParallelIndexing->addChildTasks(
				std::make_shared<TaskDecoratorRepeat>(TaskDecoratorRepeat::CONDITION_WHILE_SUCCESS, Task::STATE_SUCCESS)->addChildTask(
					std::make_shared<TaskBuildIndex>(indexerThreadCount, indexerCommandList, storageProvider, multiProcess)
				)
			);
		}
		// add task for merging the intermediate storages
		taskParallelIndexing->addTask(
			std::make_shared<TaskGroupSequence>()->addChildTasks(
				std::make_shared<TaskDecoratorRepeat>(TaskDecoratorRepeat::CONDITION_WHILE_SUCCESS, Task::STATE_SUCCESS)->addChildTask(
					std::make_shared<TaskReturnSuccessWhile<int>>("indexer_count", TaskReturnSuccessWhile<int>::CONDITION_EQUALS, 0)
				),
				std::make_shared<TaskDecoratorRepeat>(TaskDecoratorRepeat::CONDITION_WHILE_SUCCESS, Task::STATE_SUCCESS)->addChildTask(
					std::make_shared<TaskGroupSelector>()->addChildTasks(
						std::make_shared<TaskMergeStorages>(storageProvider),
						std::make_shared<TaskReturnSuccessWhile<int>>("indexer_count", TaskReturnSuccessWhile<int>::CONDITION_GREATER_THAN, 0)
					)
				)
			)
		);
		// add task for injecting the intermediate storages into the persistent storage
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
		// add task that notifies the user of what's going on
		taskSequential->addTask( // we don't need to hide this dialog again, because it's overridden by other dialogs later on.
			std::make_shared<TaskShowStatusDialog>("Finish Indexing", "Saving\nRemaining Data")
		);

		// add task that injects the remaining intermediate storages into the persistent storage
		taskSequential->addTask(
			std::make_shared<TaskDecoratorRepeat>(TaskDecoratorRepeat::CONDITION_WHILE_SUCCESS, Task::STATE_SUCCESS)->addChildTask(
				std::make_shared<TaskInjectStorage>(storageProvider, m_storage)
			)
		);
	}

	taskSequential->addTask(std::make_shared<TaskFinishParsing>(m_storage.get(), m_storageAccessProxy));

	Task::dispatch(taskSequential);
}

bool Project::hasCxxSourceGroup() const
{
	for (const std::shared_ptr<SourceGroup>& sourceGroup: m_sourceGroups)
	{
		if (sourceGroup->getLanguage() == LANGUAGE_C || sourceGroup->getLanguage() == LANGUAGE_CPP)
		{
			return true;
		}
	}
	return false;
}

bool Project::didFileChange(const FileInfo& info) const
{
	FileInfo diskFileInfo = FileSystem::getFileInfoForPath(info.path);
	if (diskFileInfo.lastWriteTime > info.lastWriteTime)
	{
		std::shared_ptr<TextAccess> storedFileContent = m_storage->getFileContent(info.path);
		std::shared_ptr<TextAccess> diskFileContent = TextAccess::createFromFile(diskFileInfo.path);

		const std::vector<std::string>& diskFileLines = diskFileContent->getAllLines();
		const std::vector<std::string>& storedFileLines = storedFileContent->getAllLines();

		if (diskFileLines.size() == storedFileLines.size())
		{
			for (size_t i = 0; i < diskFileLines.size(); i++)
			{
				if (diskFileLines[i] != storedFileLines[i])
				{
					return true;
				}
			}
			return false;
		}
		return true;
	}
	return false;
}
