#ifndef TASK_GROUP_H
#define TASK_GROUP_H

#include <memory>
#include <vector>

#include "utility/scheduling/Task.h"

class TaskGroup
	: public Task
{
public:
	TaskGroup();
	virtual ~TaskGroup();

	void addTask(std::shared_ptr<Task> task);

protected:
	std::vector<std::shared_ptr<Task>> m_tasks;
};

#endif // TASK_GROUP_H
