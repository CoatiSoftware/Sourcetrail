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

	virtual void enter();
	virtual TaskState update();
	virtual void exit();

	virtual void interrupt();
	virtual void revert();

private:
	std::function<void()> m_func;
};

#endif // LAMBDA_TASK_H
