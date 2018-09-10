#include "utility/scheduling/TaskFindKeyOnBlackboard.h"

#include "utility/scheduling/Blackboard.h"

TaskFindKeyOnBlackboard::TaskFindKeyOnBlackboard(const std::string& key)
	: m_key(key)
{
}

void TaskFindKeyOnBlackboard::doEnter(std::shared_ptr<Blackboard> blackboard)
{
}

Task::TaskState TaskFindKeyOnBlackboard::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	return (blackboard->exists(m_key)) ? STATE_SUCCESS : STATE_FAILURE;
}

void TaskFindKeyOnBlackboard::doExit(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskFindKeyOnBlackboard::doReset(std::shared_ptr<Blackboard> blackboard)
{
}
