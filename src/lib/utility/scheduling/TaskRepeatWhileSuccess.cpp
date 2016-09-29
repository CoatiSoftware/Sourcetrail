#include "utility/scheduling/TaskRepeatWhileSuccess.h"

TaskRepeatWhileSuccess::TaskRepeatWhileSuccess(TaskState exitState)
	: m_exitState(exitState)
{
}

void TaskRepeatWhileSuccess::setTask(std::shared_ptr<Task> task)
{
	if (task)
	{
		m_taskRunner = std::make_shared<TaskRunner>(task);
	}
}

void TaskRepeatWhileSuccess::doEnter(std::shared_ptr<Blackboard> blackboard)
{
}

Task::TaskState TaskRepeatWhileSuccess::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	TaskState state = m_taskRunner->update(blackboard);

	if (state == Task::STATE_SUCCESS)
	{
		m_taskRunner->reset();
		state = Task::STATE_RUNNING;
	}
	else if (state == Task::STATE_FAILURE)
	{
		state = m_exitState;
	}

	return state;
}

void TaskRepeatWhileSuccess::doExit(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskRepeatWhileSuccess::doReset(std::shared_ptr<Blackboard> blackboard)
{
	m_taskRunner->reset();
}
