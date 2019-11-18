#ifndef TASK_DECORATOR_H
#define TASK_DECORATOR_H

#include <memory>

#include "Task.h"

class TaskRunner;

class TaskDecorator
	: public Task
	, public std::enable_shared_from_this<TaskDecorator>
{
public:
	TaskDecorator();
	std::shared_ptr<TaskDecorator> addChildTask(std::shared_ptr<Task> child);

	virtual void setTask(std::shared_ptr<Task> task);
	void terminate() override;

protected:
	std::shared_ptr<TaskRunner> m_taskRunner;

private:
	virtual void doTerminate();
};

#endif	  // TASK_DECORATOR_H
