#ifndef TASK_MERGE_STORAGES_H
#define TASK_MERGE_STORAGES_H

#include <vector>

#include "../utility/scheduling/Task.h"

class StorageProvider;

class TaskMergeStorages: public Task
{
public:
	TaskMergeStorages(std::shared_ptr<StorageProvider> storageProvider);

private:
	void doEnter(std::shared_ptr<Blackboard> blackboard) override;
	TaskState doUpdate(std::shared_ptr<Blackboard> blackboard) override;
	void doExit(std::shared_ptr<Blackboard> blackboard) override;
	void doReset(std::shared_ptr<Blackboard> blackboard) override;

	std::shared_ptr<StorageProvider> m_storageProvider;
};

#endif	  // TASK_MERGE_STORAGES_H
