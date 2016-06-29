#include "utility/scheduling/TaskGroupParallel.h"

TaskGroupParallel::TaskGroupParallel()
{
}

TaskGroupParallel::~TaskGroupParallel()
{
}

void TaskGroupParallel::enter()
{
	m_interrupt = false;
	m_running = false;
	m_activeTaskCount = 0;
}

Task::TaskState TaskGroupParallel::update()
{
	if (!m_running)
	{
		for (size_t i = 0; i < m_tasks.size(); i++)
		{
			m_threads.push_back(std::thread(&TaskGroupParallel::processTaskThreaded, this, m_tasks[i]));

			std::lock_guard<std::mutex> lock(m_activeTaskCountMutex);
			m_activeTaskCount++;
		}
		m_running = true;
	}

	int activeTaskCount = 0;
	{
		std::lock_guard<std::mutex> lock(m_activeTaskCountMutex);
		activeTaskCount = m_activeTaskCount;
	}

	if (activeTaskCount == 0)
	{
		return (m_interrupt ? STATE_CANCELED : STATE_FINISHED);
	}

	return Task::STATE_RUNNING;
}

void TaskGroupParallel::exit()
{
	for (size_t i = 0; i < m_threads.size(); i++)
	{
		m_threads[i].join();
	}
	m_threads.clear();
}

void TaskGroupParallel::interrupt()
{
	m_interrupt = true;
}

void TaskGroupParallel::revert()
{
	m_interrupt = true;
}


void TaskGroupParallel::processTaskThreaded(std::shared_ptr<Task> task)
{
	Task::TaskState state = Task::STATE_NEW;
	while (state != Task::STATE_FINISHED && state != Task::STATE_CANCELED)
	{
		if (m_interrupt)
		{
			state = task->interruptTask();
		}
		else
		{
			state = task->processTask();
		}
	}

	{
		std::lock_guard<std::mutex> lock(m_activeTaskCountMutex);
		m_activeTaskCount--; // not safe! if exception hits this thread before this point the count is not decremented.
	}
}
