#include "project/Project.h"

#include "component/view/DialogView.h"
#include "data/indexer/IndexerCommand.h"
#include "data/indexer/IndexerCommandList.h"
#include "data/indexer/TaskBuildIndex.h"
#include "data/parser/TaskParseWrapper.h"
#include "data/storage/PersistentStorage.h"
#include "data/storage/StorageCache.h"
#include "data/storage/StorageProvider.h"
#include "data/TaskCleanStorage.h"
#include "data/TaskFinishParsing.h"
#include "data/TaskInjectStorage.h"
#include "data/TaskMergeStorages.h"
#include "data/TaskShowStatusDialog.h"
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

Project::Project(std::shared_ptr<ProjectSettings> settings, StorageCache* storageCache, bool hasGUI)
	: m_settings(settings)
	, m_storageCache(storageCache)
	, m_state(PROJECT_STATE_NOT_LOADED)
	, m_hasGUI(hasGUI)
{
}

Project::~Project()
{
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
	m_storageCache->setSubject(nullptr);

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

	try
	{
		m_storage->setup();
	}
	catch(...)
	{
		LOG_ERROR_STREAM(<< "Exception during storage loading.");

		canLoad = false;
		m_state = PROJECT_STATE_DB_CORRUPTED;
	}

	m_sourceGroups = SourceGroupFactory::getInstance()->createSourceGroups(m_settings->getAllSourceGroupSettings());

	if (canLoad)
	{
		m_storage->setMode(SqliteStorage::STORAGE_MODE_READ);
		m_storage->buildCaches();
		m_storageCache->setSubject(m_storage.get());

		if (m_hasGUI)
		{
			MessageFinishedParsing().dispatch();
		}
		MessageStatus("Finished Loading", false, false).dispatch();
	}
	else
	{
		MessageStatus("Project not loaded", false, false).dispatch();
	}

	if (m_state != PROJECT_STATE_LOADED && m_hasGUI)
	{
		MessageRefresh().dispatch();
	}
}

void Project::refresh(RefreshMode refreshMode, DialogView* dialogView)
{
	if (m_state == PROJECT_STATE_NOT_LOADED)
	{
		return;
	}

	bool needsFullRefresh = false;
	bool fullRefresh = false;
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
				"reflect the current project state. Alternatively you can also choose to just reindex updated or "
				"incomplete files. Do you want to reindex the project?";
			fullRefresh = true;
			break;

		case PROJECT_STATE_OUTVERSIONED:
			question =
				"This project was indexed with a different version of Sourcetrail. It needs to be fully reindexed to be used "
				"with this version of Sourcetrail. Do you want to reindex the project?";
			needsFullRefresh = true;
			break;

		case PROJECT_STATE_SETTINGS_UPDATED:
			question =
				"Some settings were changed, the project should be fully reindexed. Alternatively you can also choose to "
				"just reindex updated or incomplete files. "
				"Do you want to reindex the project?";
			fullRefresh = true;
			break;

		case PROJECT_STATE_NEEDS_MIGRATION:
			question =
				"This project was created with a different version of Sourcetrail. The project file needs to get updated and "
				"the project fully reindexed. Do you want to update the project file and reindex the project?";
			needsFullRefresh = true;

		case PROJECT_STATE_DB_CORRUPTED:
			question =
				"There was a problem loading the index of this project. The project needs to get fully reindexed. "
				"Do you want to reindex the project?";
			needsFullRefresh = true;

		default:
			break;
	}

	if (question.size() && m_hasGUI)
	{
		std::vector<std::string> options = { "Yes", "No" };
		int result = dialogView->confirm(question, options);

		if (result == 1)
		{
			return;
		}
	}

	if (ApplicationSettings::getInstance()->getLoggingEnabled() &&
		ApplicationSettings::getInstance()->getVerboseIndexerLoggingEnabled() && m_hasGUI)
	{
		std::vector<std::string> options = { "Yes", "No" };
		int result = dialogView->confirm(
			"Warning: You are about to index your project with the \"verbose indexer logging\" setting "
			"enabled. This will cause a significant slowdown in indexing performance. Do you want to proceed?",
			options
		);

		if (result == 1)
		{
			return;
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
	for (const std::shared_ptr<SourceGroup>& sourceGroup : m_sourceGroups)
	{
		if (!sourceGroup->prepareIndexing())
		{
			return;
		}

		sourceGroup->fetchAllSourceFilePaths();
	}

	if (needsFullRefresh || fullRefresh)
	{
		refreshMode = REFRESH_ALL_FILES;
	}
	else if (refreshMode == REFRESH_NONE)
	{
		refreshMode = REFRESH_UPDATED_FILES;
	}

	RefreshInfo info = getRefreshInfo(refreshMode);

	if (m_hasGUI)
	{
		std::vector<RefreshMode> enabledModes = { REFRESH_ALL_FILES };
		if (!needsFullRefresh)
		{
			enabledModes.insert(enabledModes.end(), { REFRESH_UPDATED_FILES, REFRESH_UPDATED_AND_INCOMPLETE_FILES });
		}

		dialogView->startIndexingDialog(this, enabledModes, info);
	}
	else
	{
		buildIndex(info, dialogView);
	}
}

RefreshInfo Project::getRefreshInfo(RefreshMode mode) const
{
	switch (mode)
	{
	case REFRESH_NONE:
		return RefreshInfo();

	case REFRESH_UPDATED_FILES:
		return getRefreshInfoForUpdatedFiles();

	case REFRESH_UPDATED_AND_INCOMPLETE_FILES:
		return getRefreshInfoForIncompleteFiles();

	case REFRESH_ALL_FILES:
		return getRefreshInfoForAllFiles();
	}
}

void Project::buildIndex(const RefreshInfo& info, DialogView* dialogView)
{
	if (info.mode == REFRESH_NONE || (!info.filesToClear.size() && !info.filesToIndex.size()))
	{
		if (m_hasGUI)
		{
			dialogView->hideDialogs();
		}
		else
		{
			MessageFinishedParsing().dispatch();
		}

		MessageStatus("Nothing to refresh, all files are up-to-date.").dispatch();
		return;
	}

	MessageStatus("Preparing Indexing", false, true).dispatch();
	MessageClearErrorCount().dispatch();

	dialogView->showUnknownProgressDialog("Preparing Indexing", "Setting up Indexers");

	if (info.mode == REFRESH_ALL_FILES)
	{
		m_storage->clear();
	}

	m_storageCache->clear();

	m_storage->setProjectSettingsText(TextAccess::createFromFile(getProjectSettingsFilePath())->getText());

	std::shared_ptr<TaskGroupSequence> taskSequential = std::make_shared<TaskGroupSequence>();

	// add task for clearing the database
	if (info.filesToClear.size())
	{
		taskSequential->addTask(std::make_shared<TaskCleanStorage>(
			m_storage.get(),
			utility::toVector(info.filesToClear)
		));
	}

	std::shared_ptr<IndexerCommandList> indexerCommandList = std::make_shared<IndexerCommandList>();
	for (const std::shared_ptr<SourceGroup>& sourceGroup : m_sourceGroups)
	{
		for (const std::shared_ptr<IndexerCommand>& command : sourceGroup->getIndexerCommands(info.filesToIndex))
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
	else
	{
		dialogView->hideUnknownProgressDialog();
	}

	taskSequential->addTask(std::make_shared<TaskFinishParsing>(m_storage.get(), m_storageCache));

	Task::dispatch(taskSequential);

	m_storageCache->setSubject(m_storage.get());
	m_state = PROJECT_STATE_LOADED;
}

std::set<FilePath> Project::getAllSourceFilePaths() const
{
	std::set<FilePath> allSourceFilePaths;

	for (const std::shared_ptr<SourceGroup>& sourceGroup: m_sourceGroups)
	{
		utility::append(allSourceFilePaths, sourceGroup->getAllSourceFilePaths());
	}

	return allSourceFilePaths;
}

RefreshInfo Project::getRefreshInfoForUpdatedFiles() const
{
	std::set<FilePath> unchangedFilePaths;
	std::set<FilePath> changedFilePaths;

	for (const FileInfo& info: m_storage->getFileInfoForAllFiles())
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
		else // file has been removed
		{
			changedFilePaths.insert(info.path);
		}
	}

	std::set<FilePath> filesToClear = changedFilePaths;

	// handle referencing paths
	utility::append(filesToClear, m_storage->getReferencing(changedFilePaths));

	// handle referenced paths
	std::set<FilePath> allSourceFilePaths = getAllSourceFilePaths();
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
			filesToClear.insert(path);
		}
	}

	for (const FilePath& path: unchangedFilePaths)
	{
		staticSourceFiles.erase(path);
	}

	std::set<FilePath> filesToAdd = staticSourceFiles;

	std::set<FilePath> staticSourceFilePaths;
	for (const FilePath& path: allSourceFilePaths)
	{
		if (filesToClear.find(path) == filesToClear.end() && filesToAdd.find(path) == filesToAdd.end())
		{
			staticSourceFilePaths.insert(path);
		}
	}

	RefreshInfo info;
	info.mode = REFRESH_UPDATED_FILES;
	info.filesToClear = filesToClear;

	for (const std::shared_ptr<SourceGroup>& sourceGroup: m_sourceGroups)
	{
		utility::append(info.filesToIndex, sourceGroup->getSourceFilePathsToIndex(staticSourceFilePaths));
	}

	return info;
}

RefreshInfo Project::getRefreshInfoForIncompleteFiles() const
{
	RefreshInfo info = getRefreshInfoForUpdatedFiles();
	info.mode = REFRESH_UPDATED_AND_INCOMPLETE_FILES;

	std::set<FilePath> incompleteFiles;
	for (const FilePath& path: m_storage->getIncompleteFiles())
	{
		if (info.filesToClear.find(path) == info.filesToClear.end())
		{
			incompleteFiles.insert(path);
		}
	}

	if (incompleteFiles.size())
	{
		utility::append(incompleteFiles, m_storage->getReferencing(incompleteFiles));
		utility::append(info.filesToClear, incompleteFiles);

		std::set<FilePath> staticSourceFilePaths = getAllSourceFilePaths();
		for (const FilePath& path: incompleteFiles)
		{
			staticSourceFilePaths.erase(path);
		}

		for (const std::shared_ptr<SourceGroup>& sourceGroup: m_sourceGroups)
		{
			utility::append(info.filesToIndex, sourceGroup->getSourceFilePathsToIndex(staticSourceFilePaths));
		}
	}

	return info;
}

RefreshInfo Project::getRefreshInfoForAllFiles() const
{
	RefreshInfo info;
	info.mode = REFRESH_ALL_FILES;
	info.filesToIndex = getAllSourceFilePaths();
	return info;
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
