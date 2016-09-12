#ifndef TASK_INJECT_STORAGE_H
#define TASK_INJECT_STORAGE_H

#include <vector>

#include "utility/scheduling/Task.h"

class Storage;
class StorageProvider;

class TaskInjectStorage
	: public Task
{
public:
	TaskInjectStorage(
		std::shared_ptr<StorageProvider> storageProvider,
		std::shared_ptr<Storage> target
	);

private:
	virtual void doEnter(std::shared_ptr<Blackboard> blackboard);
	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blackboard);
	virtual void doExit(std::shared_ptr<Blackboard> blackboard);
	virtual void doReset(std::shared_ptr<Blackboard> blackboard);

	std::shared_ptr<StorageProvider> m_storageProvider;
	std::shared_ptr<Storage> m_target;
};

#endif // TASK_INJECT_STORAGE_H
