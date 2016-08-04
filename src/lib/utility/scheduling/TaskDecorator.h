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

	void setTask(std::shared_ptr<Task> task);

protected:
	std::shared_ptr<Task> m_task;
};

#endif // TASK_DECORATOR_H
