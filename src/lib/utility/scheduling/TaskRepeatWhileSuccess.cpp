#include "utility/scheduling/TaskRepeatWhileSuccess.h"

TaskRepeatWhileSuccess::TaskRepeatWhileSuccess()
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
		state = Task::STATE_RUNNING;
		m_taskRunner->reset();
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
