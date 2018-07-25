#ifndef TASK_PARSE_WRAPPER_H
#define TASK_PARSE_WRAPPER_H

#include <memory>

#include "utility/scheduling/Task.h"
#include "utility/scheduling/TaskRunner.h"
#include "utility/scheduling/TaskDecorator.h"
#include "utility/TimeStamp.h"

class DialogView;
class FileRegister;
class PersistentStorage;

class TaskParseWrapper
	: public TaskDecorator
{
public:
	TaskParseWrapper(std::weak_ptr<PersistentStorage> storage, std::shared_ptr<DialogView> dialogView);

private:
	virtual void doEnter(std::shared_ptr<Blackboard> blackboard);
	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blackboard);
	virtual void doExit(std::shared_ptr<Blackboard> blackboard);
	virtual void doReset(std::shared_ptr<Blackboard> blackboard);

	std::weak_ptr<PersistentStorage> m_storage;
	std::shared_ptr<DialogView> m_dialogView;

	TimeStamp m_start;
};

#endif // TASK_PARSE_WRAPPER_H
