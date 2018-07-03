#ifndef TASK_GROUP_SEQUENCE_H
#define TASK_GROUP_SEQUENCE_H

#include "utility/scheduling/TaskGroup.h"
#include "utility/scheduling/TaskRunner.h"

class TaskGroupSequence
	: public TaskGroup
{
public:
	TaskGroupSequence();

	virtual void addTask(std::shared_ptr<Task> task);

private:
	void doEnter(std::shared_ptr<Blackboard> blackboard) override;
	TaskState doUpdate(std::shared_ptr<Blackboard> blackboard) override;
	void doExit(std::shared_ptr<Blackboard> blackboard) override;
	void doReset(std::shared_ptr<Blackboard> blackboard) override;
	void doTerminate() override;

	std::vector<std::shared_ptr<TaskRunner>> m_taskRunners;
	int m_taskIndex;
};

#endif // TASK_GROUP_SEQUENCE_H
