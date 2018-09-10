#ifndef TASK_FILL_INDEXER_COMMAND_QUEUE_H
#define TASK_FILL_INDEXER_COMMAND_QUEUE_H

#include <queue>

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageInterruptTasks.h"
#include "utility/scheduling/Task.h"

#include "data/indexer/interprocess/InterprocessIndexerCommandManager.h"

class IndexerCommandProvider;

class TaskFillIndexerCommandsQueue
	: public Task
	, public MessageListener<MessageInterruptTasks>
{
public:
	TaskFillIndexerCommandsQueue(
		const std::string& appUUID,
		std::unique_ptr<IndexerCommandProvider> indexerCommandProvider,
		size_t maximumQueueSize
	);

protected:
	void doEnter(std::shared_ptr<Blackboard> blackboard) override;
	TaskState doUpdate(std::shared_ptr<Blackboard> blackboard) override;
	void doExit(std::shared_ptr<Blackboard> blackboard) override;
	void doReset(std::shared_ptr<Blackboard> blackboard) override;

	void handleMessage(MessageInterruptTasks* message) override;

	void fillCommandQueue();

private:
	std::unique_ptr<IndexerCommandProvider> m_indexerCommandProvider;
	InterprocessIndexerCommandManager m_indexerCommandManager;
	const size_t m_maximumQueueSize;
	std::queue<FilePath> m_filePathQueue;
	std::mutex m_commandsMutex;
};

#endif // TASK_FILL_INDEXER_COMMAND_QUEUE_H
