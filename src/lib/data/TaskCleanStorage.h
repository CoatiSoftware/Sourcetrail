#ifndef TASK_CLEAN_STORAGE_H
#define TASK_CLEAN_STORAGE_H

#include <vector>

#include "utility/scheduling/Task.h"
#include "utility/TimeStamp.h"

class DialogView;
class FilePath;
class PersistentStorage;

class TaskCleanStorage
	: public Task
{
public:
	TaskCleanStorage(
		PersistentStorage* storage,
		const std::vector<FilePath>& filePaths,
		bool clearAllErrors
	);

private:
	void doEnter(std::shared_ptr<Blackboard> blackboard) override;
	TaskState doUpdate(std::shared_ptr<Blackboard> blackboard) override;
	void doExit(std::shared_ptr<Blackboard> blackboard) override;
	void doReset(std::shared_ptr<Blackboard> blackboard) override;

	PersistentStorage* m_storage;
	std::vector<FilePath> m_filePaths;
	bool m_clearAllErrors;

	TimeStamp m_start;
};

#endif // TASK_CLEAN_STORAGE_H
