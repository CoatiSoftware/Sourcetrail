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
	: m_enterCalled(false)
	, m_exitCalled(false)
{
}

Task::~Task()
{
}

Task::TaskState Task::update()
{
	if (!m_enterCalled)
	{
		doEnter();
		m_enterCalled = true;
	}

	TaskState state = doUpdate();

	if (state != STATE_RUNNING && !m_exitCalled)
	{
		doExit();
		m_exitCalled = true;
	}

	return state;
}

void Task::reset()
{
	doReset();
	m_enterCalled = false;
	m_exitCalled = false;
}
