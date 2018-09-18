#include "TaskGroup.h"

TaskGroup::TaskGroup()
{
}

std::shared_ptr<TaskGroup> TaskGroup::addChildTasks(std::shared_ptr<Task> child1)
{
	addTask(child1);
	return shared_from_this();
}

std::shared_ptr<TaskGroup> TaskGroup::addChildTasks(std::shared_ptr<Task> child1, std::shared_ptr<Task> child2)
{
	addTask(child1);
	addTask(child2);
	return shared_from_this();
}

std::shared_ptr<TaskGroup> TaskGroup::addChildTasks(std::shared_ptr<Task> child1, std::shared_ptr<Task> child2, std::shared_ptr<Task> child3)
{
	addTask(child1);
	addTask(child2);
	addTask(child3);
	return shared_from_this();
}

void TaskGroup::terminate()
{
	doTerminate();
}
