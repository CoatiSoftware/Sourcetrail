#include "TaskRunner.h"

#include "logging.h"
#include "Blackboard.h"
#include "TaskScheduler.h"

TaskRunner::TaskRunner(std::shared_ptr<Task> task)
	: m_task(task)
	, m_reset(false)
{
}

Task::TaskState TaskRunner::update(std::shared_ptr<Blackboard> blackboard)
{
	if (!blackboard)
	{
		if (!m_blackboard)
		{
			m_blackboard = std::make_shared<Blackboard>();
		}

		blackboard = m_blackboard;
	}

	try
	{
		if (m_reset)
		{
			m_task->reset(blackboard);
			m_reset = false;
		}

		return m_task->update(blackboard);
	}
	catch (std::exception& e)
	{
		LOG_ERROR(e.what());
	}
	catch (...)
	{
		LOG_ERROR("Unknown exception thrown during task running");
	}

	TaskScheduler::getInstance()->terminateRunningTasks();
	return Task::STATE_FAILURE;
}

void TaskRunner::reset()
{
	m_reset = true;
}

void TaskRunner::terminate()
{
	if (m_task)
	{
		m_task->terminate();
	}
}
