#ifndef TASK_DECORATOR_H
#define TASK_DECORATOR_H

#include <memory>

#include "utility/scheduling/Task.h"

class TaskDecorator
	: public Task
{
public:
	TaskDecorator();
	virtual ~TaskDecorator();

	virtual void setTask(std::shared_ptr<Task> task) = 0;
};

#endif // TASK_DECORATOR_H
