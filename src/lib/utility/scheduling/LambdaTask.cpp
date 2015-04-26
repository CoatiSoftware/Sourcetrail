#include "utility/scheduling/LambdaTask.h"

LambdaTask::LambdaTask(std::function<void()> func)
	: m_func(func)
{
}

LambdaTask::~LambdaTask()
{
}

void LambdaTask::perform()
{
	m_func();
}
