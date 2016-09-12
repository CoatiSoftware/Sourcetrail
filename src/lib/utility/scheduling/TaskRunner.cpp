#include "utility/scheduling/TaskRunner.h"

TaskRunner::TaskRunner(std::shared_ptr<Task> task)
	: m_task(task)
	, m_reset(false)
{
}

TaskRunner::~TaskRunner()
{
}

Task::TaskState TaskRunner::update(std::shared_ptr<Blackboard> blackboard)
{
	if (m_reset)
	{
		m_task->reset(blackboard);
		m_reset = false;
	}

	return m_task->update(blackboard);
}

void TaskRunner::reset()
{
	m_reset = true;
}


