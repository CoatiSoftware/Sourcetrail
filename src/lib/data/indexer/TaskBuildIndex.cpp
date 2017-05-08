#include "data/indexer/TaskBuildIndex.h"

#include "utility/AppPath.h"
#include "utility/file/FileRegister.h"
#include "utility/file/FileRegisterStateData.h"
#include "utility/logging/FileLogger.h"
#include "utility/scheduling/Blackboard.h"
#include "utility/UserPaths.h"
#include "utility/utilityApp.h"

#include "Application.h"
#include "component/view/DialogView.h"
#include "data/indexer/IndexerFactory.h"
#include "data/indexer/IndexerCommandList.h"
#include "data/indexer/IndexerComposite.h"
#include "data/indexer/interprocess/InterprocessIndexer.h"
#include "data/StorageProvider.h"
#include "settings/ApplicationSettings.h"

#if _WIN32
const std::string TaskBuildIndex::s_processName("sourcetrail_indexer.exe");
#else
const std::string TaskBuildIndex::s_processName("sourcetrail_indexer");
#endif

TaskBuildIndex::TaskBuildIndex(
	unsigned int processCount,
	std::shared_ptr<IndexerCommandList> indexerCommandList,
	std::shared_ptr<StorageProvider> storageProvider,
	std::shared_ptr<FileRegisterStateData> fileRegisterStateData,
	bool multiProcessIndexing
)
	: m_indexerCommandList(indexerCommandList)
	, m_storageProvider(storageProvider)
	, m_fileRegisterStateData(fileRegisterStateData)
	, m_multiProcessIndexing(multiProcessIndexing)
	, m_interprocessIndexerCommandManager(Application::getUUID(), 0, true)
	, m_interprocessIndexingStatusManager(Application::getUUID(), 0, true)
	, m_processCount(processCount)
	, m_interrupted(false)
	, m_lastCommandCount(0)
	, m_runningThreadCount(0)
{
}

void TaskBuildIndex::doEnter(std::shared_ptr<Blackboard> blackboard)
{
	updateIndexingDialog(blackboard, FilePath());

	{
		std::lock_guard<std::mutex> lock(blackboard->getMutex());
		blackboard->set("indexer_count", (int)m_processCount);
	}

	// move indexer commands to shared memory
	m_lastCommandCount = m_indexerCommandList->size();
	m_interprocessIndexerCommandManager.setIndexerCommands(m_indexerCommandList->getAllCommands());

	std::string logFilePath;
	Logger* logger = LogManager::getInstance()->getLoggerByType("FileLogger");
	if (logger)
	{
		logFilePath = dynamic_cast<FileLogger*>(logger)->getLogFilePath().str();
	}

	// start indexer processes
	for (unsigned int i = 0; i < m_processCount; i++)
	{
		const int processId = i + 1; // 0 remains reserved for the main process

		m_interprocessIntermediateStorageManagers.push_back(
			std::make_shared<InterprocessIntermediateStorageManager>(Application::getUUID(), processId, true)
		);

		if (m_multiProcessIndexing)
		{
			m_processThreads.push_back(new std::thread(&TaskBuildIndex::runIndexerProcess, this, processId, logFilePath));
		}
		else
		{
			m_processThreads.push_back(new std::thread(&TaskBuildIndex::runIndexerThread, this, processId));
		}
	}
}

Task::TaskState TaskBuildIndex::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	size_t runningThreadCount = 0;
	{
		std::lock_guard<std::mutex> lock(m_runningThreadCountMutex);
		runningThreadCount = m_runningThreadCount;
	}

	size_t commandCount = m_interprocessIndexerCommandManager.indexerCommandCount();
	if (commandCount != m_lastCommandCount)
	{
		updateIndexingDialog(blackboard, m_interprocessIndexingStatusManager.getCurrentlyIndexedSourceFilePath());
		m_lastCommandCount = commandCount;
	}

	if (commandCount == 0 && runningThreadCount == 0)
	{
		return STATE_FAILURE;
	}
	else if (m_interrupted)
	{
		std::lock_guard<std::mutex> lock(blackboard->getMutex());
		blackboard->set("interrupted_indexing", true);

		// clear indexer commands, this causes the indexer processes to return when finished with respective current indexer commands
		m_interprocessIndexerCommandManager.clearIndexerCommands();
		return STATE_FAILURE;
	}

	fetchIntermediateStorages(blackboard);

	const int SLEEP_TIME_MS = 100;
	std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));

	return STATE_RUNNING;
}

void TaskBuildIndex::doExit(std::shared_ptr<Blackboard> blackboard)
{
	for (auto processThread : m_processThreads)
	{
		processThread->join();
		delete processThread;
	}
	m_processThreads.clear();

	fetchIntermediateStorages(blackboard);

	std::vector<FilePath> crashedFiles = m_interprocessIndexingStatusManager.getCrashedSourceFilePaths();
	if (crashedFiles.size())
	{
		std::shared_ptr<IntermediateStorage> is = std::make_shared<IntermediateStorage>();
		for (auto path : crashedFiles)
		{
			is->addError("The translation unit threw an exception during indexing. Please check if the source file "
				"conforms to the specified language standard and all necessary options are defined within your project "
				"setup.", path, 1, 1, true, true);
			LOG_INFO_STREAM(<< "crashed translation unit: " << path.str());
		}
		m_storageProvider->insert(is);
	}

	std::lock_guard<std::mutex> lock(blackboard->getMutex());
	blackboard->set("indexer_count", 0);
}

void TaskBuildIndex::doReset(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskBuildIndex::terminate()
{
	m_interrupted = true;
	utility::killRunningProcesses();
}

void TaskBuildIndex::handleMessage(MessageInterruptTasks* message)
{
	m_interrupted = true;
}

void TaskBuildIndex::runIndexerProcess(int processId, const std::string& logFilePath)
{
	{
		std::lock_guard<std::mutex> lock(m_runningThreadCountMutex);
		m_runningThreadCount++;
	}

	FilePath indexerProcessPath(AppPath::getAppPath() + s_processName);
	if (!indexerProcessPath.exists())
	{
		m_interrupted = true;
		LOG_ERROR("Cannot start indexer process because executable is missing at \"" + indexerProcessPath.str() + "\"");
		return;
	}

	std::string command = "\"" + indexerProcessPath.str() + "\"";
	command += " " + std::to_string(processId);
	command += " " + Application::getUUID();
	command += " \"" + AppPath::getAppPath() + "\"";
	command += " \"" + UserPaths::getUserDataPath().str() + "\"";

	if (logFilePath.size())
	{
		command += " \"" + logFilePath + "\"";
	}

	int result = 1;
	while (result != 0 && !m_interrupted)
	{
		result = utility::executeProcessAndGetExitCode(command.c_str(), "", -1);

		LOG_INFO_STREAM(<< "Indexer process " << processId << " returned with " + std::to_string(result));
	}

	{
		std::lock_guard<std::mutex> lock(m_runningThreadCountMutex);
		m_runningThreadCount--;
	}
}

void TaskBuildIndex::runIndexerThread(int processId)
{
	{
		std::lock_guard<std::mutex> lock(m_runningThreadCountMutex);
		m_runningThreadCount++;
	}

	InterprocessIndexer indexer(Application::getUUID(), processId);
	indexer.work();

	{
		std::lock_guard<std::mutex> lock(m_runningThreadCountMutex);
		m_runningThreadCount--;
	}
}

void TaskBuildIndex::fetchIntermediateStorages(std::shared_ptr<Blackboard> blackboard)
{
	int newlyIndexedCount = 0;

	for (std::shared_ptr<InterprocessIntermediateStorageManager> storageManager: m_interprocessIntermediateStorageManagers)
	{
		while (int storageCount = storageManager->getIntermediateStorageCount())
		{
			LOG_INFO_STREAM(<< storageManager->getProcessId() << " - storage count: " << storageCount);
			m_storageProvider->insert(storageManager->popIntermediateStorage());
			++newlyIndexedCount;

			updateIndexingDialog(blackboard, m_interprocessIndexingStatusManager.getCurrentlyIndexedSourceFilePath());
		}
	}

	if (newlyIndexedCount > 0)
	{
		std::lock_guard<std::mutex> lock(blackboard->getMutex());

		int indexedSourceFileCount = 0;
		blackboard->get("indexed_source_file_count", indexedSourceFileCount);
		blackboard->set("indexed_source_file_count", indexedSourceFileCount + newlyIndexedCount);
	}
}

void TaskBuildIndex::updateIndexingDialog(std::shared_ptr<Blackboard> blackboard, const FilePath& sourcePath)
{
	// TODO: factor in unindexed files...
	int sourceFileCount = 0;
	int indexedSourceFileCount = 0;
	{
		std::lock_guard<std::mutex> lock(blackboard->getMutex());
		blackboard->get("source_file_count", sourceFileCount);
		blackboard->get("indexed_source_file_count", indexedSourceFileCount);
	}

	if (std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView())
	{
		dialogView->updateIndexingDialog(
			indexedSourceFileCount, sourceFileCount, sourcePath.str()
		);
	}
}
