#ifndef TASK_DECORATOR_H
#define TASK_DECORATOR_H

#include <memory>

#include "utility/scheduling/Task.h"

class TaskDecorator
	: public Task
	, public std::enable_shared_from_this<TaskDecorator>
{
public:
	TaskDecorator();
	virtual ~TaskDecorator();
	std::shared_ptr<TaskDecorator> addChildTask(std::shared_ptr<Task> child);

	virtual void setTask(std::shared_ptr<Task> task) = 0;
	virtual void terminate();

private:
	virtual void doTerminate() = 0;
};

#endif // TASK_DECORATOR_H
