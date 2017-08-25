#ifndef TASK_BUILD_INDEX_H
#define TASK_BUILD_INDEX_H

#include <thread>

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageInterruptTasks.h"
#include "utility/scheduling/Task.h"

#include "data/indexer/interprocess/InterprocessIndexerCommandManager.h"
#include "data/indexer/interprocess/InterprocessIndexingStatusManager.h"
#include "data/indexer/interprocess/InterprocessIntermediateStorageManager.h"

class DialogView;
class StorageProvider;
class IndexerCommandList;

class TaskBuildIndex
	: public Task
	, public MessageListener<MessageInterruptTasks>
{
public:
	TaskBuildIndex(
		unsigned int processCount,
		std::shared_ptr<IndexerCommandList> indexerCommandList,
		std::shared_ptr<StorageProvider> storageProvider,
		bool multiProcessIndexing
	);

protected:
	virtual void doEnter(std::shared_ptr<Blackboard> blackboard);
	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blackboard);
	virtual void doExit(std::shared_ptr<Blackboard> blackboard);
	virtual void doReset(std::shared_ptr<Blackboard> blackboard);
	virtual void terminate();

	virtual void handleMessage(MessageInterruptTasks* message);

	void runIndexerProcess( int processId, const std::string& logFilePath);
	void runIndexerThread(int processId);
	bool fetchIntermediateStorages(std::shared_ptr<Blackboard> blackboard);
	void updateIndexingDialog(std::shared_ptr<Blackboard> blackboard, const std::vector<FilePath>& sourcePaths);

	static const std::string s_processName;

	std::shared_ptr<IndexerCommandList> m_indexerCommandList;
	std::shared_ptr<StorageProvider> m_storageProvider;
	bool m_multiProcessIndexing;

	InterprocessIndexerCommandManager m_interprocessIndexerCommandManager;
	InterprocessIndexingStatusManager m_interprocessIndexingStatusManager;

	unsigned int m_processCount;
	bool m_interrupted;
	size_t m_lastCommandCount;
	size_t m_indexingFileCount;

	// store as plain pointers to avoid deallocation issues when closing app during indexing
	std::vector<std::thread*> m_processThreads;
	std::vector<std::shared_ptr<InterprocessIntermediateStorageManager>> m_interprocessIntermediateStorageManagers;

	size_t m_runningThreadCount;
	std::mutex m_runningThreadCountMutex;
};

#endif // TASK_PARSE_H
