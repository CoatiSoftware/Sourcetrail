#ifndef TASK_CLEAN_STORAGE_H
#define TASK_CLEAN_STORAGE_H

#include <vector>

#include "utility/file/FilePath.h"
#include "utility/scheduling/Task.h"
#include "utility/TimePoint.h"

class PersistentStorage;

class TaskCleanStorage
	: public Task
{
public:
	TaskCleanStorage(
		PersistentStorage* storage,
		const std::vector<FilePath>& filePaths
	);

	virtual void enter();
	virtual TaskState update();
	virtual void exit();

	virtual void interrupt();
	virtual void revert();

private:
	PersistentStorage* m_storage;
	std::vector<FilePath> m_filePaths;
	const size_t m_fileCount;

	TimePoint m_start;
};

#endif // TASK_PARSE_CXX_H
