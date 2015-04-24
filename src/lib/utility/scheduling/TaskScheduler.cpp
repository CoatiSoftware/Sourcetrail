#include "utility/scheduling/TaskScheduler.h"

#include <chrono>
#include <thread>

#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageStatus.h"

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
	m_tasks.push(task);
}

void TaskScheduler::interruptCurrentTask()
{
	std::lock_guard<std::mutex> lock(m_tasksMutex);
	m_interruptTask = true;
}

void TaskScheduler::startSchedulerLoopThreaded()
{
	std::thread(&TaskScheduler::startSchedulerLoop, this).detach();

	std::lock_guard<std::mutex> lock(m_threadMutex);
	m_threadIsRunning = true;
}

void TaskScheduler::startSchedulerLoop()
{
	{
		std::lock_guard<std::mutex> lock(m_loopMutex);

		if (m_loopIsRunning)
		{
			LOG_ERROR("Loop is already running");
			return;
		}

		m_loopIsRunning = true;
	}

	while (true)
	{
		updateTasks();

		{
			std::lock_guard<std::mutex> lock(m_loopMutex);

			if (!m_loopIsRunning)
			{
				break;
			}
		}

		const int SLEEP_TIME_MS = 25;
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
			LOG_WARNING("Loop is not running");
		}

		m_loopIsRunning = false;
	}

	interruptCurrentTask();

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
	return m_tasks.size();
}

std::shared_ptr<TaskScheduler> TaskScheduler::s_instance;

TaskScheduler::TaskScheduler()
	: m_loopIsRunning(false)
	, m_threadIsRunning(false)
	, m_interruptTask(false)
{
}

void TaskScheduler::updateTasks()
{
	std::lock_guard<std::mutex> lock(m_tasksMutex);

	bool interrupt = m_interruptTask;

	while (m_tasks.size())
	{
		std::shared_ptr<Task> task = m_tasks.front();

		m_tasksMutex.unlock();
		Task::TaskState state = task->process(interrupt);
		m_tasksMutex.lock();

		if (state == Task::STATE_FINISHED || state == Task::STATE_CANCELED)
		{
			m_tasks.pop();
		}

		interrupt = m_interruptTask;
	}

	m_interruptTask = false;
}

void TaskScheduler::handleMessage(MessageInterruptTasks* message)
{
	interruptCurrentTask();

	std::lock_guard<std::mutex> lock(m_tasksMutex);
	if (m_tasks.size())
	{
		MessageStatus("Stop running tasks...").dispatch();
	}
}
