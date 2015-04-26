#ifndef LAMBDA_TASK_H
#define LAMBDA_TASK_H

#include <functional>

#include "utility/scheduling/SimpleTask.h"

class LambdaTask
	: public SimpleTask
{
public:
	LambdaTask(std::function<void()> func);
	virtual ~LambdaTask();

	virtual void perform();

private:
	std::function<void()> m_func;
};

#endif // LAMBDA_TASK_H
