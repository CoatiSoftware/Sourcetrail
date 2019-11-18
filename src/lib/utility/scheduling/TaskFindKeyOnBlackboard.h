#ifndef TASK_FIND_KEY_ON_BLACKBOARD_H
#define TASK_FIND_KEY_ON_BLACKBOARD_H

#include <string>

#include "Task.h"

class Blackboard;

class TaskFindKeyOnBlackboard: public Task
{
public:
	TaskFindKeyOnBlackboard(const std::string& valueName);

private:
	void doEnter(std::shared_ptr<Blackboard> blackboard) override;
	TaskState doUpdate(std::shared_ptr<Blackboard> blackboard) override;
	void doExit(std::shared_ptr<Blackboard> blackboard) override;
	void doReset(std::shared_ptr<Blackboard> blackboard) override;

	const std::string m_key;
};

#endif	  // TASK_FIND_KEY_ON_BLACKBOARD_H
