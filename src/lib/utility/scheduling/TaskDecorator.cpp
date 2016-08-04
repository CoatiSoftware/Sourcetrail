#include "utility/scheduling/TaskDecorator.h"

TaskDecorator::TaskDecorator()
{
}

TaskDecorator::~TaskDecorator()
{
}

void TaskDecorator::setTask(std::shared_ptr<Task> task)
{
	m_task = task;
}
