#include "utility/scheduling/TaskDecorator.h"

TaskDecorator::TaskDecorator()
{
}

TaskDecorator::~TaskDecorator()
{
}

std::shared_ptr<TaskDecorator> TaskDecorator::addChildTask(std::shared_ptr<Task> child)
{
	setTask(child);
	return shared_from_this();
}
