#ifndef TASK_FINISH_PARSING_H
#define TASK_FINISH_PARSING_H

#include <vector>

#include "utility/file/FilePath.h"
#include "utility/scheduling/Task.h"

class DialogView;
class FileRegister;
class PersistentStorage;
class StorageAccess;

class TaskFinishParsing
	: public Task
{
public:
	TaskFinishParsing(
		PersistentStorage* storage,
		StorageAccess* storageAccess,
		std::shared_ptr<FileRegister> fileRegister,
		DialogView* dialogView
	);

	virtual ~TaskFinishParsing();

private:
	virtual void doEnter(std::shared_ptr<Blackboard> blackboard);
	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blackboard);
	virtual void doExit(std::shared_ptr<Blackboard> blackboard);
	virtual void doReset(std::shared_ptr<Blackboard> blackboard);

	PersistentStorage* m_storage;
	StorageAccess* m_storageAccess;
	std::shared_ptr<FileRegister> m_fileRegister;
	DialogView* m_dialogView;
};

#endif // TASK_FINISH_PARSING_H
