#include "utility/scheduling/Task.h"

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
	: m_enterCalled(false)
	, m_exitCalled(false)
{
}

Task::~Task()
{
}

Task::TaskState Task::update(std::shared_ptr<Blackboard> blackboard)
{
	if (!m_enterCalled)
	{
		doEnter(blackboard);
		m_enterCalled = true;
	}

	TaskState state = doUpdate(blackboard);

	if (state != STATE_RUNNING && !m_exitCalled)
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

