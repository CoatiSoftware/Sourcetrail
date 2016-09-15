#include "utility/scheduling/TaskScheduler.h"

#include <chrono>
#include <thread>

#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/scheduling/Blackboard.h"
#include "utility/ScopedFunctor.h"

std::shared_ptr<TaskScheduler> TaskScheduler::getInstance()
{
	if (!s_instance)
	{
		s_instance = std::shared_ptr<TaskScheduler>(new TaskScheduler());
	}

	return s_instance;
}

void TaskScheduler::pushTask(std::shared_ptr<Task> task)
{
	std::lock_guard<std::mutex> lock(m_tasksMutex);
	m_taskRunners.push_back(std::make_shared<TaskRunner>(task));
}

void TaskScheduler::pushNextTask(std::shared_ptr<Task> task)
{
	std::lock_guard<std::mutex> lock(m_tasksMutex);

	if (m_taskRunners.size() == 0)
	{
		m_taskRunners.push_front(std::make_shared<TaskRunner>(task));
	}
	else
	{
		m_taskRunners.insert(m_taskRunners.begin() + 1, std::make_shared<TaskRunner>(task));
	}
}

void TaskScheduler::startSchedulerLoopThreaded()
{
	std::thread(&TaskScheduler::startSchedulerLoop, this).detach();

	std::lock_guard<std::mutex> lock(m_threadMutex);
	m_threadIsRunning = true;
}

void TaskScheduler::startSchedulerLoop()
{
	const int SLEEP_TIME_MS = 25;

	{
		std::lock_guard<std::mutex> lock(m_loopMutex);

		if (m_loopIsRunning)
		{
			LOG_ERROR("Unable to start task scheduler. Loop is already running.");
			return;
		}

		m_loopIsRunning = true;
	}

	while (true)
	{
		processTasks();

		{
			std::lock_guard<std::mutex> lock(m_loopMutex);

			if (!m_loopIsRunning)
			{
				break;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));
	}

	{
		std::lock_guard<std::mutex> lock(m_threadMutex);
		if (m_threadIsRunning)
		{
			m_threadIsRunning = false;
		}
	}
}

void TaskScheduler::stopSchedulerLoop()
{
	{
		std::lock_guard<std::mutex> lock(m_loopMutex);

		if (!m_loopIsRunning)
		{
			LOG_WARNING("Unable to stop task scheduler. Loop is not running.");
		}

		m_loopIsRunning = false;
	}

	while (true)
	{
		{
			std::lock_guard<std::mutex> lock(m_threadMutex);
			if (!m_threadIsRunning)
			{
				break;
			}
		}

		const int SLEEP_TIME_MS = 25;
		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));
	}
}

bool TaskScheduler::loopIsRunning() const
{
	std::lock_guard<std::mutex> lock(m_loopMutex);
	return m_loopIsRunning;
}

bool TaskScheduler::hasTasksQueued() const
{
	std::lock_guard<std::mutex> lock(m_tasksMutex);
	return m_taskRunners.size();
}

std::shared_ptr<TaskScheduler> TaskScheduler::s_instance;

TaskScheduler::TaskScheduler()
	: m_loopIsRunning(false)
	, m_threadIsRunning(false)
{
}

void TaskScheduler::processTasks()
{
	std::lock_guard<std::mutex> lock(m_tasksMutex);

	while (m_taskRunners.size())
	{
		std::shared_ptr<TaskRunner> runner = m_taskRunners.front();

		{
			m_tasksMutex.unlock();
			ScopedFunctor functor([this](){
				m_tasksMutex.lock();
			});

			std::shared_ptr<Blackboard> blackboard = std::make_shared<Blackboard>();
			while (true)
			{
				if (runner->update(blackboard) != Task::STATE_RUNNING)
				{
					break;
				}
			}
		}

		m_taskRunners.pop_front();
	}
}
