#ifndef LAMBDA_TASK_H
#define LAMBDA_TASK_H

#include <functional>

#include "utility/scheduling/Task.h"

class TaskLambda
	: public Task
{
public:
	TaskLambda(std::function<void()> func);
	virtual ~TaskLambda();

private:
	virtual void doEnter();
	virtual TaskState doUpdate();
	virtual void doExit();
	virtual void doReset();

	std::function<void()> m_func;
};

#endif // LAMBDA_TASK_H
