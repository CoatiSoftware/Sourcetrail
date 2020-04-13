#ifndef TASK_INJECT_STORAGE_H
#define TASK_INJECT_STORAGE_H

#include <vector>

#include "../utility/messaging/type/indexing/MessageIndexingInterrupted.h"
#include "../utility/messaging/MessageListener.h"
#include "../utility/scheduling/Task.h"

class Storage;
class StorageProvider;

class TaskInjectStorage
	: public Task
	, public MessageListener<MessageIndexingInterrupted>
{
public:
	TaskInjectStorage(std::shared_ptr<StorageProvider> storageProvider, std::weak_ptr<Storage> target);

private:
	void doEnter(std::shared_ptr<Blackboard> blackboard) override;
	TaskState doUpdate(std::shared_ptr<Blackboard> blackboard) override;
	void doExit(std::shared_ptr<Blackboard> blackboard) override;
	void doReset(std::shared_ptr<Blackboard> blackboard) override;

	void handleMessage(MessageIndexingInterrupted* message) override;

	std::shared_ptr<StorageProvider> m_storageProvider;
	std::weak_ptr<Storage> m_target;
};

#endif	  // TASK_INJECT_STORAGE_H
