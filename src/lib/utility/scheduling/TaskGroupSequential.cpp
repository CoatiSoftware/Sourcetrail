#include "utility/scheduling/TaskGroupSequential.h"

TaskGroupSequential::TaskGroupSequential()
	: m_taskIndex(-1)
{
}

TaskGroupSequential::~TaskGroupSequential()
{
}

void TaskGroupSequential::enter()
{
}

Task::TaskState TaskGroupSequential::update()
{
	if (!m_tasks.size())
	{
		return Task::STATE_FINISHED;
	}

	if (m_taskIndex < 0 || m_tasks[m_taskIndex]->getState() != Task::STATE_RUNNING)
	{
		m_taskIndex++;
	}

	std::shared_ptr<Task> task = m_tasks[m_taskIndex];

	TaskState state = task->process(false);

	if (state == Task::STATE_FINISHED && size_t(m_taskIndex + 1) == m_tasks.size())
	{
		return Task::STATE_FINISHED;
	}

	return Task::STATE_RUNNING;
}

void TaskGroupSequential::exit()
{
}

void TaskGroupSequential::interrupt()
{
	if (m_taskIndex >= 0 && size_t(m_taskIndex) < m_tasks.size())
	{
		for (int i = m_taskIndex; i >= 0; i--)
		{
			m_tasks[i]->process(true);
		}
	}
}

void TaskGroupSequential::revert()
{
	for (int i = m_tasks.size() - 1; i >= 0; i--)
	{
		m_tasks[i]->process(true);
	}
}
