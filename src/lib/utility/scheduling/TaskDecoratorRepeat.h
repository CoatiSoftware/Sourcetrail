#ifndef TASK_DECORATOR_REPEAT_H
#define TASK_DECORATOR_REPEAT_H

#include <vector>

#include "utility/scheduling/TaskDecorator.h"
#include "utility/scheduling/TaskRunner.h"

class TaskDecoratorRepeat
	: public TaskDecorator
{
public:
	enum ConditionType
	{
		CONDITION_WHILE_SUCCESS
	};

	TaskDecoratorRepeat(ConditionType condition, TaskState exitState);

private:
	virtual void doEnter(std::shared_ptr<Blackboard> blackboard);
	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blackboard);
	virtual void doExit(std::shared_ptr<Blackboard> blackboard);
	virtual void doReset(std::shared_ptr<Blackboard> blackboard);

	const ConditionType m_condition;
	const TaskState m_exitState;
};

#endif // TASK_DECORATOR_REPEAT_H
