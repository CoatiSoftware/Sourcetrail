#include "utility/scheduling/TaskLambda.h"

TaskLambda::TaskLambda(std::function<void()> func)
	: m_func(func)
{
}

TaskLambda::~TaskLambda()
{
}

void TaskLambda::doEnter()
{
}

Task::TaskState TaskLambda::doUpdate()
{
	m_func();
	return STATE_SUCCESS;
}

void TaskLambda::doExit()
{
}

void TaskLambda::doReset()
{
}
