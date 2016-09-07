#include "utility/scheduling/TaskRunner.h"

TaskRunner::TaskRunner(std::shared_ptr<Task> task)
	: m_task(task)
	, m_reset(false)
{
}

TaskRunner::~TaskRunner()
{
}

//Task::TaskState TaskRunner::getState() const
//{
//	return m_task->getState();
//}

Task::TaskState TaskRunner::update()
{
	if (m_reset)
	{
		m_task->reset();
		m_reset = false;
	}

	return m_task->update();
}

void TaskRunner::reset()
{
	m_reset = true;
}


