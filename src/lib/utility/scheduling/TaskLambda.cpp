#include "utility/scheduling/TaskLambda.h"

TaskLambda::TaskLambda(std::function<void()> func)
	: m_func(func)
{
}

TaskLambda::~TaskLambda()
{
}

void TaskLambda::enter()
{
}

Task::TaskState TaskLambda::update()
{
	m_func();
	return Task::STATE_FINISHED;
}

void TaskLambda::exit()
{
}

void TaskLambda::interrupt()
{
}

void TaskLambda::revert()
{
}

void TaskLambda::abort()
{
}
