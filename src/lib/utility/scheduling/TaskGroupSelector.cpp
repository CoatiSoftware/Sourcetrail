#include "utility/scheduling/TaskGroupSelector.h"

TaskGroupSelector::TaskGroupSelector()
{
}

void TaskGroupSelector::addTask(std::shared_ptr<Task> task)
{
	m_taskRunners.push_back(std::make_shared<TaskRunner>(task));
}

void TaskGroupSelector::doEnter(std::shared_ptr<Blackboard> blackboard)
{
	m_taskIndex = 0;
}

Task::TaskState TaskGroupSelector::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	if (m_taskIndex >= int(m_taskRunners.size()))
	{
		return STATE_FAILURE;
	}
	else if (m_taskIndex < 0)
	{
		return STATE_SUCCESS;
	}

	TaskState state = m_taskRunners[m_taskIndex]->update(blackboard);

	if (state == STATE_FAILURE)
	{
		m_taskIndex++;
	}
	else if (state == STATE_SUCCESS)
	{
		m_taskIndex = -1;
	}
	else if (state == STATE_HOLD)
	{
		return STATE_HOLD;
	}

	return STATE_RUNNING;
}

void TaskGroupSelector::doExit(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskGroupSelector::doReset(std::shared_ptr<Blackboard> blackboard)
{
	for (size_t i = 0; i < m_taskRunners.size(); i++)
	{
		m_taskRunners[i]->reset();
	}
}

void TaskGroupSelector::doTerminate()
{
	for (size_t i = 0; i < m_taskRunners.size(); i++)
	{
		m_taskRunners[i]->terminate();
	}
}
