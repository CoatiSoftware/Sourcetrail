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

	virtual void enter();
	virtual TaskState update();
	virtual void exit();

	virtual void interrupt();
	virtual void revert();
	virtual void abort();

private:
	PersistentStorage* m_storage;
	std::vector<FilePath> m_filePaths;
	DialogView* m_dialogView;
};

#endif // TASK_PARSE_CXX_H
