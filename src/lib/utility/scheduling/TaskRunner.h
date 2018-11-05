#ifndef TASK_RUNNER_H
#define TASK_RUNNER_H

#include <memory>

#include "Task.h"

class TaskRunner
{
public:
	TaskRunner(std::shared_ptr<Task> task);

	Task::TaskState update(Id schedulerId);
	Task::TaskState update(std::shared_ptr<Blackboard> blackboard);
	void reset();
	void terminate(); // caution: this should only be called just before quitting the app.

private:
	std::shared_ptr<Task> m_task;
	bool m_reset;

	// Only created by the first TaskRunner in the hierarchy, then passed down.
	std::shared_ptr<Blackboard> m_blackboard;
};

#endif // TASK_RUNNER_H
