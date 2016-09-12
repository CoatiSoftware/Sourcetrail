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
	virtual void doEnter(std::shared_ptr<Blackboard> blackboard);
	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blackboard);
	virtual void doExit(std::shared_ptr<Blackboard> blackboard);
	virtual void doReset(std::shared_ptr<Blackboard> blackboard);

	std::vector<std::shared_ptr<TaskRunner>> m_taskRunners;
	int m_taskIndex;
};

#endif // TASK_GROUP_SEQUENTIAL_H
