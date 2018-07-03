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
	void doEnter(std::shared_ptr<Blackboard> blackboard) override;
	TaskState doUpdate(std::shared_ptr<Blackboard> blackboard) override;
	void doExit(std::shared_ptr<Blackboard> blackboard) override;
	void doReset(std::shared_ptr<Blackboard> blackboard) override;

	std::shared_ptr<StorageProvider> m_storageProvider;
	std::shared_ptr<Storage> m_target;
};

#endif // TASK_INJECT_STORAGE_H
