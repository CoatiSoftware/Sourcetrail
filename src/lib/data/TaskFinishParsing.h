#ifndef TASK_FINISH_PARSING_H
#define TASK_FINISH_PARSING_H

#include <vector>

#include "utility/scheduling/Task.h"

class DialogView;
class FileRegister;
class PersistentStorage;
class StorageAccess;

class TaskFinishParsing
	: public Task
{
public:
	TaskFinishParsing(std::shared_ptr<PersistentStorage> storage);

	void terminate() override;

private:
	void doEnter(std::shared_ptr<Blackboard> blackboard) override;
	TaskState doUpdate(std::shared_ptr<Blackboard> blackboard) override;
	void doExit(std::shared_ptr<Blackboard> blackboard) override;
	void doReset(std::shared_ptr<Blackboard> blackboard) override;

	std::shared_ptr<PersistentStorage> m_storage;
};

#endif // TASK_FINISH_PARSING_H
