#include "utility/scheduling/TaskGroupSequential.h"
#include <iostream>

TaskGroupSequential::TaskGroupSequential()
{
}

TaskGroupSequential::~TaskGroupSequential()
{
}

void TaskGroupSequential::addTask(std::shared_ptr<Task> task)
{
	m_taskRunners.push_back(std::make_shared<TaskRunner>(task));
}

void TaskGroupSequential::doEnter()
{
	m_taskIndex = 0;
}

Task::TaskState TaskGroupSequential::doUpdate()
{
	if (m_taskIndex >= int(m_taskRunners.size()))
	{
		return STATE_SUCCESS;
	}
	else if (m_taskIndex < 0)
	{
		return STATE_FAILURE;
	}

	TaskState state = m_taskRunners[m_taskIndex]->update();

	if (state == STATE_SUCCESS)
	{
		m_taskIndex++;
	}
	else if (state == STATE_FAILURE)
	{
		m_taskIndex = -1;
	}

	return STATE_RUNNING;
}

void TaskGroupSequential::doExit()
{
}

void TaskGroupSequential::doReset()
{
	for (size_t i = 0; i < m_taskRunners.size(); i++)
	{
		m_taskRunners[i]->reset();
	}
}
