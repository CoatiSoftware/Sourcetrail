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

	virtual void addTask(std::shared_ptr<Task> task) = 0;
};

#endif // TASK_GROUP_H
