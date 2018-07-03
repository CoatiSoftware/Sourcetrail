#ifndef TASK_FIND_VALUE_H
#define TASK_FIND_VALUE_H

#include <string>

#include "utility/scheduling/Task.h"

class Blackboard;

class TaskFindValue:
	public Task
{
public:
	TaskFindValue(const std::string& valueName);

private:
	void doEnter(std::shared_ptr<Blackboard> blackboard) override;
	TaskState doUpdate(std::shared_ptr<Blackboard> blackboard) override;
	void doExit(std::shared_ptr<Blackboard> blackboard) override;
	void doReset(std::shared_ptr<Blackboard> blackboard) override;

	const std::string m_valueName;
};

#endif // TASK_FIND_VALUE_H
