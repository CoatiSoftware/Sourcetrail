#include "utility/scheduling/Task.h"

#include "utility/logging/logging.h"
#include "utility/scheduling/TaskScheduler.h"

void Task::dispatch(std::shared_ptr<Task> task)
{
	TaskScheduler::getInstance()->pushTask(task);
}

void Task::dispatchNext(std::shared_ptr<Task> task)
{
	TaskScheduler::getInstance()->pushNextTask(task);
}

Task::Task()
	: m_state(STATE_NEW)
{
}

Task::~Task()
{
}

Task::TaskState Task::getState() const
{
	return m_state;
}

Task::TaskState Task::processTask()
{
	switch (m_state)
	{
	case STATE_NEW:
	case STATE_CANCELED:
		enter();
	case STATE_RUNNING:
		{
			TaskState newState = update();
			if (newState == STATE_NEW || newState == STATE_CANCELED)
			{
				LOG_ERROR("Task can't change to state NEW or CANCELLED");
				return m_state;
			}

			setState(newState);
			if (m_state == STATE_FINISHED)
			{
				exit();
			}
		}
		break;
	case STATE_FINISHED:
		break;
	}

	return m_state;
}

Task::TaskState Task::interruptTask()
{
	switch (m_state)
	{
	case STATE_NEW:
		abort();
		break;
	case STATE_CANCELED:
		break;
	case STATE_RUNNING:
		interrupt();
		exit();
		break;
	case STATE_FINISHED:
		revert();
		break;
	}

	setState(STATE_CANCELED);
	return m_state;
}

void Task::executeTask()
{
	TaskState state;
	do
	{
		state = processTask();
	}
	while (state != STATE_FINISHED);
}

void Task::setState(TaskState state)
{
	m_state = state;
}
