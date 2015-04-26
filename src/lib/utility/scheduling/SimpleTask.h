#ifndef SIMPLE_TASK_H
#define SIMPLE_TASK_H

#include "utility/scheduling/Task.h"

class SimpleTask
	: public Task
{
public:
	virtual void enter();
	virtual TaskState update();
	virtual void exit();

	virtual void interrupt();
	virtual void revert();

	virtual void perform() = 0;
};

#endif // SIMPLE_TASK_H
