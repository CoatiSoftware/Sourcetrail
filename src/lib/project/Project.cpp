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
#include "project/RefreshInfoGenerator.h"
#include "project/SourceGroup.h"
#include "project/SourceGroupFactory.h"
#include "settings/ApplicationSettings.h"
#include "settings/ProjectSettings.h"
#include "settings/SourceGroupStatusType.h"

#include "utility/file/FilePath.h"
#include "utility/file/FileSystem.h"
#include "utility/messaging/type/error/MessageErrorCountClear.h"
#include "utility/messaging/type/indexing/MessageIndexingFinished.h"
#include "utility/messaging/type/indexing/MessageIndexingStarted.h"
#include "utility/messaging/type/indexing/MessageIndexingStatus.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/scheduling/TaskDecoratorRepeat.h"
#include "utility/scheduling/TaskFindValue.h"
#include "utility/scheduling/TaskGroupSelector.h"
#include "utility/scheduling/TaskGroupSequence.h"
#include "utility/scheduling/TaskGroupParallel.h"
#include "utility/scheduling/TaskLambda.h"
#include "utility/scheduling/TaskReturnSuccessWhile.h"
#include "utility/scheduling/TaskSetValue.h"
#include "utility/ScopedFunctor.h"
#include "utility/text/TextAccess.h"
#include "utility/utility.h"
#include "utility/utilityApp.h"
#include "utility/utilityFile.h"
#include "utility/utilityString.h"
#include "Application.h"

const std::wstring Project::PROJECT_FILE_EXTENSION = L".srctrlprj";
const std::wstring Project::BOOKMARK_DB_FILE_EXTENSION = L".srctrlbm";
const std::wstring Project::INDEX_DB_FILE_EXTENSION = L".srctrldb";
const std::wstring Project::TEMP_INDEX_DB_FILE_EXTENSION = L".srctrldb_tmp";

Project::Project(std::shared_ptr<ProjectSettings> settings, StorageCache* storageCache, bool hasGUI)
	: m_settings(settings)
	, m_storageCache(storageCache)
	, m_state(PROJECT_STATE_NOT_LOADED)
	, m_isIndexing(false)
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

bool Project::isIndexing() const
{
	return m_isIndexing;
}

bool Project::settingsEqualExceptNameAndLocation(const ProjectSettings& otherSettings) const
{
	return m_settings->equalsExceptNameAndLocation(otherSettings);
}

void Project::setStateOutdated()
{
	if (m_state == PROJECT_STATE_LOADED)
	{
		m_state = PROJECT_STATE_OUTDATED;
	}
}

void Project::load()
{
	if (m_isIndexing)
	{
		MessageStatus(L"Cannot load another project while indexing.", true, false).dispatch();
		return;
	}

	m_storageCache->clear();
	m_storageCache->setSubject(nullptr);

	if (!m_settings->reload())
	{
		return;
	}

	const FilePath projectSettingsPath = m_settings->getFilePath();

	{
		const FilePath dbPath = projectSettingsPath.replaceExtension(INDEX_DB_FILE_EXTENSION);
		const FilePath tempDbPath = projectSettingsPath.replaceExtension(TEMP_INDEX_DB_FILE_EXTENSION);
		if (tempDbPath.exists())
		{
			if (dbPath.exists())
			{
				if (Application::getInstance()->getDialogView(DialogView::UseCase::GENERAL)->confirm(
					"Sourcetrail has been closed unexpectedly while indexing this project. You can either choose to keep the data that has "
					"already been indexed or discard that data and restore the state of your project before indexing?",
					{ "Keep and Continue", "Discard and Restore" }) == 0)
				{
					LOG_INFO("Switching to temporary indexing data on user's decision");
					FileSystem::remove(dbPath);
					FileSystem::rename(tempDbPath, dbPath);
				}
				else
				{
					LOG_INFO("Discarding temporary indexing data on user's decision");
					FileSystem::remove(tempDbPath);
				}
			}
			else
			{
				LOG_INFO("Switching to temporary indexing data because no other persistent data was found");
				FileSystem::rename(tempDbPath, dbPath);
			}
		}
	}

	m_storage = std::make_shared<PersistentStorage>(
		projectSettingsPath.replaceExtension(INDEX_DB_FILE_EXTENSION),
		projectSettingsPath.replaceExtension(BOOKMARK_DB_FILE_EXTENSION)
	);

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
	catch (...)
	{
		LOG_ERROR("Exception has been encountered while loading the project.");

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
			MessageIndexingFinished().dispatch();
		}
		MessageStatus(L"Finished Loading", false, false).dispatch();
	}
	else
	{
		switch (m_state)
		{
		case PROJECT_STATE_NEEDS_MIGRATION:
			MessageStatus(L"Project could not be loaded and needs to be re-indexed after automatic migration to latest "
				"version.", false, false).dispatch();
			break;
		case PROJECT_STATE_EMPTY:
			MessageStatus(L"Project could not load any symbols because the index database is empty. Please re-index the "
				"project.", false, false).dispatch();
			break;
		case PROJECT_STATE_OUTVERSIONED:
			MessageStatus(L"Project could not be loaded because the indexed data format is incompatible to the current "
				"version of Sourcetrail. Please re-index the project.", false, false).dispatch();
			break;
		default:
			MessageStatus(L"Project could not be loaded.", false, false).dispatch();
		}
	}

	if (m_state != PROJECT_STATE_LOADED && m_hasGUI)
	{
		MessageRefresh().dispatch();
	}
}

void Project::refresh(RefreshMode refreshMode, std::shared_ptr<DialogView> dialogView)
{
	if (m_isIndexing)
	{
		MessageStatus(L"Cannot refresh the project while indexing.", true, false).dispatch();
		return;
	}

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
				"This project was indexed with a different version of Sourcetrail. It needs to be fully reindexed to "
				"be used with this version of Sourcetrail. Do you want to reindex the project?";
			needsFullRefresh = true;
			break;

		case PROJECT_STATE_NEEDS_MIGRATION:
			question =
				"This project was created with a different version and uses an old project file format. "
				"The project can still be opened and used with this version, but needs to be fully reindexed. "
				"Do you want Sourcetrail to update the project file and reindex the project?";
			needsFullRefresh = true;
			break;

		case PROJECT_STATE_DB_CORRUPTED:
			question =
				"There was a problem loading the index of this project. The project needs to get fully reindexed. "
				"Do you want to reindex the project?";
			needsFullRefresh = true;
			break;

		default:
			break;
	}

	if (question.size() && m_hasGUI)
	{
		if (dialogView->confirm(question, { "Reindex", "Cancel" }) == 1)
		{
			return;
		}
	}

	if (ApplicationSettings::getInstance()->getLoggingEnabled() &&
		ApplicationSettings::getInstance()->getVerboseIndexerLoggingEnabled() && m_hasGUI)
	{
		if (dialogView->confirm(
				"Warning: You are about to index your project with the \"verbose indexer logging\" setting "
				"enabled. This will cause a significant slowdown in indexing performance. Do you want to proceed?",
				{ "Yes", "No" }) == 1)
		{
			return;
		}
	}

	dialogView->showUnknownProgressDialog(L"Preparing Project", L"Processing Files");
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

		dialogView->startIndexingDialog(this, enabledModes, info, [this, dialogView](const RefreshInfo& info) { buildIndex(info, dialogView); });
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
		return RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(m_sourceGroups, m_storage);

	case REFRESH_UPDATED_AND_INCOMPLETE_FILES:
		return RefreshInfoGenerator::getRefreshInfoForIncompleteFiles(m_sourceGroups, m_storage);

	case REFRESH_ALL_FILES:
		return RefreshInfoGenerator::getRefreshInfoForAllFiles(m_sourceGroups);
	}
}

void Project::buildIndex(const RefreshInfo& info, std::shared_ptr<DialogView> dialogView)
{
	if (m_isIndexing)
	{
		MessageStatus(L"Cannot refresh project while indexing.", true, false).dispatch();
		return;
	}

	if (info.mode != REFRESH_ALL_FILES && info.filesToClear.empty() && info.filesToIndex.empty())
	{
		if (m_hasGUI)
		{
			dialogView->clearDialogs();
		}
		else
		{
			MessageIndexingFinished().dispatch();
		}

		MessageStatus(L"Nothing to refresh, all files are up-to-date.").dispatch();
		return;
	}

	MessageStatus(L"Preparing Indexing", false, true).dispatch();
	MessageErrorCountClear().dispatch();

	dialogView->showUnknownProgressDialog(L"Preparing Indexing", L"Setting up Indexers");
	MessageIndexingStatus(true, 0).dispatch();

	m_storageCache->clear();
	m_storageCache->setSubject(m_storage.get());

	const FilePath indexDbFilePath = m_storage->getIndexDbFilePath();
	const FilePath tempIndexDbFilePath = indexDbFilePath.replaceExtension(TEMP_INDEX_DB_FILE_EXTENSION);

	if (info.mode != REFRESH_ALL_FILES)
	{
		FileSystem::copyFile(indexDbFilePath, tempIndexDbFilePath);
	}

	std::shared_ptr<PersistentStorage> tempStorage = std::make_shared<PersistentStorage>(tempIndexDbFilePath, m_storage->getBookmarkDbFilePath());
	tempStorage->setup();

	std::shared_ptr<TaskGroupSequence> taskSequential = std::make_shared<TaskGroupSequence>();

	bool hideable = m_state == PROJECT_STATE_LOADED || m_state == PROJECT_STATE_OUTDATED;
	dialogView->setDialogsHideable(hideable);

	if (info.mode != REFRESH_ALL_FILES && (info.filesToClear.size() || info.nonIndexedFilesToClear.size()))
	{
		taskSequential->addTask(std::make_shared<TaskCleanStorage>(
			tempStorage,
			utility::toVector(utility::concat(info.filesToClear, info.nonIndexedFilesToClear)),
			info.mode == REFRESH_UPDATED_AND_INCOMPLETE_FILES
		));
	}

	tempStorage->setProjectSettingsText(TextAccess::createFromFile(getProjectSettingsFilePath())->getText());
	tempStorage->updateVersion();

	std::shared_ptr<IndexerCommandList> indexerCommandList = std::make_shared<IndexerCommandList>();
	for (const std::shared_ptr<SourceGroup>& sourceGroup : m_sourceGroups)
	{
		if (sourceGroup->getStatus() == SOURCE_GROUP_STATUS_ENABLED)
		{
			for (const std::shared_ptr<IndexerCommand>& command : sourceGroup->getIndexerCommands(info.filesToIndex))
			{
				indexerCommandList->addCommand(command);
			}
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

		std::shared_ptr<TaskParseWrapper> taskParserWrapper = std::make_shared<TaskParseWrapper>(tempStorage);

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
							std::make_shared<TaskInjectStorage>(storageProvider, tempStorage),
							// continuing when indexer count is greater than zero, even if there are no storages right now.
							std::make_shared<TaskReturnSuccessWhile<int>>("indexer_count", TaskReturnSuccessWhile<int>::CONDITION_GREATER_THAN, 0)
						)
					)
				)
			)
		);
		// add task that notifies the user of what's going on
		taskSequential->addTask( // we don't need to hide this dialog again, because it's overridden by other dialogs later on.
			std::make_shared<TaskLambda>([dialogView]() {
				dialogView->showUnknownProgressDialog(L"Finish Indexing", L"Saving\nRemaining Data");
			})
		);

		// add task that injects the remaining intermediate storages into the persistent storage
		taskSequential->addTask(
			std::make_shared<TaskDecoratorRepeat>(TaskDecoratorRepeat::CONDITION_WHILE_SUCCESS, Task::STATE_SUCCESS)->addChildTask(
				std::make_shared<TaskInjectStorage>(storageProvider, tempStorage)
			)
		);
	}
	else
	{
		dialogView->hideUnknownProgressDialog();
	}

	taskSequential->addTask(std::make_shared<TaskFinishParsing>(tempStorage));

	taskSequential->addTask(std::make_shared<TaskGroupSelector>()->addChildTasks(
		std::make_shared<TaskGroupSequence>()->addChildTasks(
			std::make_shared<TaskFindValue>("keep_database"),
			std::make_shared<TaskLambda>([this]() {
				Task::dispatch(std::make_shared<TaskLambda>([this]() {
					swapToTempStorage();
					m_state = PROJECT_STATE_LOADED;
				}));
			})
		),
		std::make_shared<TaskGroupSequence>()->addChildTasks(
			std::make_shared<TaskFindValue>("discard_database"),
			std::make_shared<TaskLambda>([this]() {
				Task::dispatch(std::make_shared<TaskLambda>([this]() {
					discardTempStorage();
				}));
			})
		)
	));

	taskSequential->addTask(std::make_shared<TaskLambda>([dialogView, this]() {
		m_isIndexing = false;
		dialogView->setDialogsHideable(false);

		MessageIndexingFinished().dispatch();
	}));

	taskSequential->setIsBackgroundTask(true);
	Task::dispatch(taskSequential);

	m_isIndexing = true;
	MessageIndexingStarted().dispatch();
}

void Project::swapToTempStorage()
{
	LOG_INFO("Switching to temporary indexing data");

	const FilePath indexDbFilePath = m_storage->getIndexDbFilePath();
	const FilePath tempIndexDbFilePath = indexDbFilePath.replaceExtension(TEMP_INDEX_DB_FILE_EXTENSION);
	const FilePath bookmarkDbFilePath = m_storage->getBookmarkDbFilePath();

	m_storage.reset();
	FileSystem::remove(indexDbFilePath);
	FileSystem::rename(tempIndexDbFilePath, indexDbFilePath);
	m_storage = std::make_shared<PersistentStorage>(indexDbFilePath, bookmarkDbFilePath);
	m_storage->setup();

	//std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView(DialogView::UseCase::INDEXING);
	//dialogView->showUnknownProgressDialog(L"Finish Indexing", L"Building caches");
	m_storage->buildCaches();
	//dialogView->hideUnknownProgressDialog();

	m_storageCache->setSubject(m_storage.get());
}

void Project::discardTempStorage()
{
	const FilePath tempIndexDbPath = m_storage->getIndexDbFilePath().replaceExtension(TEMP_INDEX_DB_FILE_EXTENSION);
	if (tempIndexDbPath.exists())
	{
		LOG_INFO("Discarding temporary indexing data");
		FileSystem::remove(tempIndexDbPath);
	}
}

bool Project::hasCxxSourceGroup() const
{
	for (const std::shared_ptr<SourceGroup>& sourceGroup: m_sourceGroups)
	{
		if (sourceGroup->getStatus() == SOURCE_GROUP_STATUS_ENABLED)
		{
			if (sourceGroup->getLanguage() == LANGUAGE_C || sourceGroup->getLanguage() == LANGUAGE_CPP)
			{
				return true;
			}
		}
	}
	return false;
}
