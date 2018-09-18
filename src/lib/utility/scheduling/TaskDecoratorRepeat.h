#ifndef TASK_DECORATOR_REPEAT_H
#define TASK_DECORATOR_REPEAT_H

#include <vector>

#include "TaskDecorator.h"
#include "TaskRunner.h"

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
	void doEnter(std::shared_ptr<Blackboard> blackboard) override;
	TaskState doUpdate(std::shared_ptr<Blackboard> blackboard) override;
	void doExit(std::shared_ptr<Blackboard> blackboard) override;
	void doReset(std::shared_ptr<Blackboard> blackboard) override;

	const ConditionType m_condition;
	const TaskState m_exitState;
};

#endif // TASK_DECORATOR_REPEAT_H
