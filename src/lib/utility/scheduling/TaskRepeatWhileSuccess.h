#ifndef TASK_REPEAT_WHILE_SUCCESS_H
#define TASK_REPEAT_WHILE_SUCCESS_H

#include <vector>

#include "utility/scheduling/TaskDecorator.h"
#include "utility/scheduling/TaskRunner.h"

class TaskRepeatWhileSuccess
	: public TaskDecorator
{
public:
	TaskRepeatWhileSuccess();

	virtual void setTask(std::shared_ptr<Task> task);

private:
	virtual void doEnter(std::shared_ptr<Blackboard> blackboard);
	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blackboard);
	virtual void doExit(std::shared_ptr<Blackboard> blackboard);
	virtual void doReset(std::shared_ptr<Blackboard> blackboard);

	std::shared_ptr<TaskRunner> m_taskRunner;
};

#endif // TASK_REPEAT_WHILE_SUCCESS_H
