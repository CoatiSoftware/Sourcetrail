#ifndef TASK_DECORATOR_DELAY_H
#define TASK_DECORATOR_DELAY_H

#include <vector>

#include "utility/scheduling/TaskDecorator.h"
#include "utility/scheduling/TaskRunner.h"
#include "utility/TimeStamp.h"

class TaskDecoratorDelay
	: public TaskDecorator
{
public:
	TaskDecoratorDelay(size_t delayMS);

private:
	virtual void doEnter(std::shared_ptr<Blackboard> blackboard);
	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blackboard);
	virtual void doExit(std::shared_ptr<Blackboard> blackboard);
	virtual void doReset(std::shared_ptr<Blackboard> blackboard);
	virtual void doTerminate();

	const size_t m_delayMS;

	TimeStamp m_start;
	bool m_delayComplete;
};

#endif // TASK_DECORATOR_DELAY_H
