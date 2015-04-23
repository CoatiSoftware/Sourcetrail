#ifndef TASK_GROUP_SEQUENTIAL_H
#define TASK_GROUP_SEQUENTIAL_H

#include "utility/scheduling/TaskGroup.h"

class TaskGroupSequential
	: public TaskGroup
{
public:
	TaskGroupSequential();
	virtual ~TaskGroupSequential();

	virtual void enter();
	virtual TaskState update();
	virtual void exit();

	virtual void interrupt();
	virtual void revert();

private:
	int m_taskIndex;
};

#endif // TASK_GROUP_SEQUENTIAL_H
