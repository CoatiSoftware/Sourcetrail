#include "TaskDecorator.h"

#include "TaskRunner.h"

TaskDecorator::TaskDecorator()
{
}

std::shared_ptr<TaskDecorator> TaskDecorator::addChildTask(std::shared_ptr<Task> child)
{
	setTask(child);
	return shared_from_this();
}

void TaskDecorator::setTask(std::shared_ptr<Task> task)
{
	if (task)
	{
		m_taskRunner = std::make_shared<TaskRunner>(task);
	}
}

void TaskDecorator::terminate()
{
	doTerminate();
}

void TaskDecorator::doTerminate()
{
	m_taskRunner->terminate();
}
