#include "utility/scheduling/TaskGroupParallel.h"

#include <chrono>
#include <thread>

#include "utility/ScopedFunctor.h"

TaskGroupParallel::TaskGroupParallel()
	: m_needsToStartThreads(true)
{
}

TaskGroupParallel::~TaskGroupParallel()
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
		m_activeTaskCount = 0;
		for (size_t i = 0; i < m_tasks.size(); i++)
		{
			m_tasks[i]->thread = std::make_shared<std::thread>(&TaskGroupParallel::processTaskThreaded, this, m_tasks[i], blackboard);
			m_tasks[i]->active = true;
			m_activeTaskCount++;
		}
	}
}

Task::TaskState TaskGroupParallel::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	if (m_tasks.size() != 0 && getActveTaskCount() > 0)
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
			m_tasks[i]->thread->join();
			m_tasks[i]->thread = std::make_shared<std::thread>(&TaskGroupParallel::processTaskThreaded, this, m_tasks[i], blackboard);
			m_tasks[i]->active = true;
			m_activeTaskCount++;
		}
	}
}

void TaskGroupParallel::processTaskThreaded(std::shared_ptr<TaskInfo> taskInfo, std::shared_ptr<Blackboard> blackboard)
{
	const int SLEEP_TIME_MS = 25;

	ScopedFunctor functor([&](){
		std::lock_guard<std::mutex> lock(m_activeTaskCountMutex);
		m_activeTaskCount--;
	});


	while (true)
	{
		TaskState state = taskInfo->taskRunner->update(blackboard);

		if (state != STATE_RUNNING)
		{
			if (state == STATE_FAILURE)
			{
				m_taskFailed = true;
			}
			taskInfo->active = false;
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));
	}
}

int TaskGroupParallel::getActveTaskCount() const
{
	std::lock_guard<std::mutex> lock(m_activeTaskCountMutex);
	return m_activeTaskCount;
}
