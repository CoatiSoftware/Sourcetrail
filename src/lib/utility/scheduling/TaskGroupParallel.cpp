#include "TaskGroupParallel.h"

#include <chrono>
#include <thread>

#include "ScopedFunctor.h"

TaskGroupParallel::TaskGroupParallel()
	: m_needsToStartThreads(true), m_activeTaskCountMutex(std::make_shared<std::mutex>())
{
}

void TaskGroupParallel::addTask(std::shared_ptr<Task> task)
{
	m_tasks.push_back(std::make_shared<TaskInfo>(std::make_shared<TaskRunner>(task)));
}

void TaskGroupParallel::doEnter(std::shared_ptr<Blackboard> blackboard)
{
	m_taskFailed = false;

	if (m_needsToStartThreads)
	{
		m_needsToStartThreads = false;
		m_activeTaskCount = static_cast<int>(m_tasks.size());
		for (size_t i = 0; i < m_tasks.size(); i++)
		{
			m_tasks[i]->active = true;
			m_tasks[i]->thread = std::make_shared<std::thread>(
				&TaskGroupParallel::processTaskThreaded,
				this,
				m_tasks[i],
				blackboard,
				m_activeTaskCountMutex);
		}
	}
}

Task::TaskState TaskGroupParallel::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(25));

	if (m_tasks.size() != 0 && getActiveTaskCount() > 0)
	{
		return STATE_RUNNING;
	}

	return (m_taskFailed ? STATE_FAILURE : STATE_SUCCESS);
}

void TaskGroupParallel::doExit(std::shared_ptr<Blackboard> blackboard)
{
	for (size_t i = 0; i < m_tasks.size(); i++)
	{
		m_tasks[i]->thread->join();
		m_tasks[i]->thread.reset();
	}
}

void TaskGroupParallel::doReset(std::shared_ptr<Blackboard> blackboard)
{
	for (size_t i = 0; i < m_tasks.size(); i++)
	{
		m_tasks[i]->taskRunner->reset();
		if (!m_tasks[i]->active)
		{
			{
				std::lock_guard<std::mutex> lock(*m_activeTaskCountMutex.get());
				m_activeTaskCount++;
			}
			m_tasks[i]->thread->join();
			m_tasks[i]->active = true;
			m_tasks[i]->thread = std::make_shared<std::thread>(
				&TaskGroupParallel::processTaskThreaded,
				this,
				m_tasks[i],
				blackboard,
				m_activeTaskCountMutex);
		}
	}
}

void TaskGroupParallel::doTerminate()
{
	for (size_t i = 0; i < m_tasks.size(); i++)
	{
		m_tasks[i]->taskRunner->terminate();
	}

	for (size_t i = 0; i < m_tasks.size(); i++)
	{
		if (m_tasks[i]->thread)
		{
			m_tasks[i]->thread->join();
			m_tasks[i]->thread.reset();
		}
	}
}

void TaskGroupParallel::processTaskThreaded(
	std::shared_ptr<TaskInfo> taskInfo,
	std::shared_ptr<Blackboard> blackboard,
	std::shared_ptr<std::mutex> activeTaskCountMutex)
{
	ScopedFunctor functor([&]() {
		std::lock_guard<std::mutex> lock(*activeTaskCountMutex.get());
		m_activeTaskCount--;
	});

	while (true)
	{
		TaskState state = taskInfo->taskRunner->update(blackboard);

		if (state == STATE_SUCCESS || state == STATE_FAILURE)
		{
			if (state == STATE_FAILURE)
			{
				m_taskFailed = true;
			}
			taskInfo->active = false;
			break;
		}
	}
}

int TaskGroupParallel::getActiveTaskCount() const
{
	std::lock_guard<std::mutex> lock(*m_activeTaskCountMutex.get());
	return m_activeTaskCount;
}
