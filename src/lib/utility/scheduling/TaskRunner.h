#ifndef TASK_RUNNER_H
#define TASK_RUNNER_H

#include <memory>

#include "utility/scheduling/Task.h"

class TaskRunner
{
public:
	TaskRunner(std::shared_ptr<Task> task);
	~TaskRunner();

	Task::TaskState update(std::shared_ptr<Blackboard> blackboard);
	void reset();

private:
	std::shared_ptr<Task> m_task;
	bool m_reset;
};

#endif // TASK_H
