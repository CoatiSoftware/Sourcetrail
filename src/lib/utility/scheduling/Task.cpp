#include "Task.h"

#include "TaskScheduler.h"

void Task::dispatch(std::shared_ptr<Task> task)
{
	TaskScheduler::getInstance()->pushTask(task);
}

void Task::dispatchNext(std::shared_ptr<Task> task)
{
	TaskScheduler::getInstance()->pushNextTask(task);
}

void Task::setIsBackgroundTask(bool background)
{
	m_isBackgroundTask = background;
}

Task::TaskState Task::update(std::shared_ptr<Blackboard> blackboard)
{
	if (!m_enterCalled)
	{
		doEnter(blackboard);
		m_enterCalled = true;
	}

	TaskState state = doUpdate(blackboard);

	if (m_isBackgroundTask && state == STATE_RUNNING)
	{
		state = STATE_HOLD;
	}

	if ((state == STATE_SUCCESS || state == STATE_FAILURE) && !m_exitCalled)
	{
		doExit(blackboard);
		m_exitCalled = true;
	}

	return state;
}

void Task::reset(std::shared_ptr<Blackboard> blackboard)
{
	doReset(blackboard);
	m_enterCalled = false;
	m_exitCalled = false;
}

void Task::terminate()
{
}
