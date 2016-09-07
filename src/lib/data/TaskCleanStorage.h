#ifndef TASK_CLEAN_STORAGE_H
#define TASK_CLEAN_STORAGE_H

#include <vector>

#include "utility/file/FilePath.h"
#include "utility/scheduling/Task.h"

class DialogView;
class PersistentStorage;

class TaskCleanStorage
	: public Task
{
public:
	TaskCleanStorage(
		PersistentStorage* storage,
		const std::vector<FilePath>& filePaths,
		DialogView* dialogView
	);

private:
	virtual void doEnter();
	virtual TaskState doUpdate();
	virtual void doExit();
	virtual void doReset();

	PersistentStorage* m_storage;
	std::vector<FilePath> m_filePaths;
	DialogView* m_dialogView;
};

#endif // TASK_CLEAN_STORAGE_H
