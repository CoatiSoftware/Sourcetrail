#ifndef LAMBDA_TASK_H
#define LAMBDA_TASK_H

#include <functional>

#include "utility/scheduling/Task.h"

class LambdaTask
	: public Task
{
public:
	LambdaTask(std::function<void()> func);
	virtual ~LambdaTask();

	virtual void enter();
	virtual TaskState update();
	virtual void exit();

	virtual void interrupt();
	virtual void revert();

private:
	std::function<void()> m_func;
};

#endif // LAMBDA_TASK_H
