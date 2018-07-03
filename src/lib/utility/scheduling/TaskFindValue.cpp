#include "utility/scheduling/TaskFindValue.h"

#include "utility/scheduling/Blackboard.h"

TaskFindValue::TaskFindValue(const std::string& valueName)
	: m_valueName(valueName)
{
}

void TaskFindValue::doEnter(std::shared_ptr<Blackboard> blackboard)
{
}

Task::TaskState TaskFindValue::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	std::lock_guard<std::mutex> lock(blackboard->getMutex());
	return (blackboard->exists(m_valueName)) ? STATE_SUCCESS : STATE_FAILURE;
}

void TaskFindValue::doExit(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskFindValue::doReset(std::shared_ptr<Blackboard> blackboard)
{
}
