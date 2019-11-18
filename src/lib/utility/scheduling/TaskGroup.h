#ifndef TASK_GROUP_H
#define TASK_GROUP_H

#include <memory>
#include <vector>

#include "Task.h"

class TaskGroup
	: public Task
	, public std::enable_shared_from_this<TaskGroup>
{
public:
	TaskGroup();
	std::shared_ptr<TaskGroup> addChildTasks(std::shared_ptr<Task> child1);
	std::shared_ptr<TaskGroup> addChildTasks(std::shared_ptr<Task> child1, std::shared_ptr<Task> child2);
	std::shared_ptr<TaskGroup> addChildTasks(
		std::shared_ptr<Task> child1, std::shared_ptr<Task> child2, std::shared_ptr<Task> child3);

	virtual void addTask(std::shared_ptr<Task> task) = 0;
	void terminate() override;

private:
	virtual void doTerminate() = 0;
};


#endif	  // TASK_GROUP_H
