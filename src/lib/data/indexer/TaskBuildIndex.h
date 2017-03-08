#ifndef TASK_BUILD_INDEX_H
#define TASK_BUILD_INDEX_H

#include "data/parser/Parser.h"
#include "utility/scheduling/Task.h"
#include "utility/messaging/type/MessageInterruptTasks.h"
#include "utility/messaging/MessageListener.h"

class CxxParser;
class DialogView;
class FileRegisterStateData;
class StorageProvider;
class IndexerCommandList;
class IndexerBase;

class TaskBuildIndex
	: public Task
	, public MessageListener<MessageInterruptTasks>
{
public:
	TaskBuildIndex(
		std::shared_ptr<IndexerCommandList> indexerCommandList,
		std::shared_ptr<StorageProvider> storageProvider,
		std::shared_ptr<FileRegisterStateData> fileRegisterStateData,
		std::shared_ptr<DialogView> dialogView
	);

protected:
	virtual void doEnter(std::shared_ptr<Blackboard> blackboard);
	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blackboard);
	virtual void doExit(std::shared_ptr<Blackboard> blackboard);
	virtual void doReset(std::shared_ptr<Blackboard> blackboard);

	virtual void handleMessage(MessageInterruptTasks* message);

	std::shared_ptr<IndexerCommandList> m_indexerCommandList;
	std::shared_ptr<StorageProvider> m_storageProvider;
	std::shared_ptr<FileRegisterStateData> m_fileRegisterStateData;
	std::shared_ptr<DialogView> m_dialogView;

	std::shared_ptr<IndexerBase> m_indexer;
};

#endif // TASK_PARSE_H
