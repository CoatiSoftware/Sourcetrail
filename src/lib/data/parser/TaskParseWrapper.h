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
	TaskParseWrapper(PersistentStorage* storage);
	virtual ~TaskParseWrapper();

private:
	virtual void doEnter(std::shared_ptr<Blackboard> blackboard);
	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blackboard);
	virtual void doExit(std::shared_ptr<Blackboard> blackboard);
	virtual void doReset(std::shared_ptr<Blackboard> blackboard);

	PersistentStorage* m_storage;

	TimeStamp m_start;
};

#endif // TASK_PARSE_WRAPPER_H
