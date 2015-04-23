#include "utility/scheduling/TaskGroup.h"

TaskGroup::TaskGroup()
{
}

TaskGroup::~TaskGroup()
{
}

void TaskGroup::addTask(std::shared_ptr<Task> task)
{
	m_tasks.push_back(task);
}
