#include "utility/scheduling/TaskLambda.h"

TaskLambda::TaskLambda(std::function<void()> func)
	: m_func(func)
{
}

TaskLambda::~TaskLambda()
{
}

void TaskLambda::doEnter(std::shared_ptr<Blackboard> blackboard)
{
}

Task::TaskState TaskLambda::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	m_func();
	return STATE_SUCCESS;
}

void TaskLambda::doExit(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskLambda::doReset(std::shared_ptr<Blackboard> blackboard)
{
}
