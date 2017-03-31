#ifndef TASK_CLEAN_STORAGE_H
#define TASK_CLEAN_STORAGE_H

#include <vector>

#include "utility/file/FilePath.h"
#include "utility/scheduling/Task.h"
#include "utility/TimePoint.h"

class DialogView;
class PersistentStorage;

class TaskCleanStorage
	: public Task
{
public:
	TaskCleanStorage(
		PersistentStorage* storage,
		const std::vector<FilePath>& filePaths
	);

private:
	virtual void doEnter(std::shared_ptr<Blackboard> blackboard);
	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blackboard);
	virtual void doExit(std::shared_ptr<Blackboard> blackboard);
	virtual void doReset(std::shared_ptr<Blackboard> blackboard);

	PersistentStorage* m_storage;
	std::vector<FilePath> m_filePaths;

	TimePoint m_start;
};

#endif // TASK_CLEAN_STORAGE_H
