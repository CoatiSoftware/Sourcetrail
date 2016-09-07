#ifndef TASK_GROUP_SEQUENTIAL_H
#define TASK_GROUP_SEQUENTIAL_H

#include "utility/scheduling/TaskGroup.h"
#include "utility/scheduling/TaskRunner.h"

class TaskGroupSequential
	: public TaskGroup
{
public:
	TaskGroupSequential();
	virtual ~TaskGroupSequential();

	virtual void addTask(std::shared_ptr<Task> task);

private:
	virtual void doEnter();
	virtual TaskState doUpdate();
	virtual void doExit();
	virtual void doReset();

	std::vector<std::shared_ptr<TaskRunner>> m_taskRunners;
	int m_taskIndex;
};

#endif // TASK_GROUP_SEQUENTIAL_H
