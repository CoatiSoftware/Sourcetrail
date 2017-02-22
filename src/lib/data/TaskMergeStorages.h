#ifndef TASK_MERGE_STORAGES_H
#define TASK_MERGE_STORAGES_H

#include <vector>

#include "utility/scheduling/Task.h"

class StorageProvider;

class TaskMergeStorages
	: public Task
{
public:
	TaskMergeStorages(
		std::shared_ptr<StorageProvider> storageProvider
	);

private:
	virtual void doEnter(std::shared_ptr<Blackboard> blackboard);
	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blackboard);
	virtual void doExit(std::shared_ptr<Blackboard> blackboard);
	virtual void doReset(std::shared_ptr<Blackboard> blackboard);

	std::shared_ptr<StorageProvider> m_storageProvider;
};

#endif // TASK_MERGE_STORAGES_H
