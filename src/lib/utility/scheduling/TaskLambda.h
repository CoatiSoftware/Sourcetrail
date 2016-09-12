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
	virtual void doEnter(std::shared_ptr<Blackboard> blackboard);
	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blackboard);
	virtual void doExit(std::shared_ptr<Blackboard> blackboard);
	virtual void doReset(std::shared_ptr<Blackboard> blackboard);

	std::function<void()> m_func;
};

#endif // LAMBDA_TASK_H
