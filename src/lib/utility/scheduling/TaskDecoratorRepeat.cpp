#include "utility/scheduling/TaskDecoratorRepeat.h"

TaskDecoratorRepeat::TaskDecoratorRepeat(ConditionType condition, TaskState exitState)
	: m_condition(condition)
	, m_exitState(exitState)
{
}

void TaskDecoratorRepeat::setTask(std::shared_ptr<Task> task)
{
	if (task)
	{
		m_taskRunner = std::make_shared<TaskRunner>(task);
	}
}

void TaskDecoratorRepeat::doEnter(std::shared_ptr<Blackboard> blackboard)
{
}

Task::TaskState TaskDecoratorRepeat::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	TaskState state = m_taskRunner->update(blackboard);

	switch (m_condition)
	{
	case CONDITION_WHILE_SUCCESS:
		if (state == Task::STATE_SUCCESS)
		{
			m_taskRunner->reset();
			state = Task::STATE_RUNNING;
		}
		else if (state == Task::STATE_FAILURE)
		{
			state = m_exitState;
		}
		break;
	}

	return state;
}

void TaskDecoratorRepeat::doExit(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskDecoratorRepeat::doReset(std::shared_ptr<Blackboard> blackboard)
{
	m_taskRunner->reset();
}
