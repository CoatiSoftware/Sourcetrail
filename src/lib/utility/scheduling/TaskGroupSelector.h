#ifndef TASK_GROUP_SELECTOR_H
#define TASK_GROUP_SELECTOR_H

#include "TaskGroup.h"
#include "TaskRunner.h"

class TaskGroupSelector: public TaskGroup
{
public:
	TaskGroupSelector();

	void addTask(std::shared_ptr<Task> task) override;

private:
	void doEnter(std::shared_ptr<Blackboard> blackboard) override;
	TaskState doUpdate(std::shared_ptr<Blackboard> blackboard) override;
	void doExit(std::shared_ptr<Blackboard> blackboard) override;
	void doReset(std::shared_ptr<Blackboard> blackboard) override;
	void doTerminate() override;

	std::vector<std::shared_ptr<TaskRunner>> m_taskRunners;
	int m_taskIndex;
};

#endif	  // TASK_GROUP_SELECTOR_H
