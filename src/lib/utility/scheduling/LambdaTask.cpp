#include "utility/scheduling/LambdaTask.h"

LambdaTask::LambdaTask(std::function<void()> func)
	: m_func(func)
{
}

LambdaTask::~LambdaTask()
{
}

void LambdaTask::enter()
{
}

Task::TaskState LambdaTask::update()
{
	m_func();

	return Task::STATE_FINISHED;
}

void LambdaTask::exit()
{
}

void LambdaTask::interrupt()
{
}

void LambdaTask::revert()
{
}
