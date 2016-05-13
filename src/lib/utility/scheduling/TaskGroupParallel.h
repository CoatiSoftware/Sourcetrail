#ifndef TASK_GROUP_PARALLEL_H
#define TASK_GROUP_PARALLEL_H

#include "utility/scheduling/TaskGroup.h"
#include <mutex>

class TaskGroupParallel
	: public TaskGroup
{
public:
	TaskGroupParallel();
	virtual ~TaskGroupParallel();

	virtual void enter();
	virtual TaskState update();
	virtual void exit();

	virtual void interrupt();
	virtual void revert();

private:
	void processTask(std::shared_ptr<Task> task);

	volatile bool m_interrupt;
	bool m_running;
	volatile int m_activeTaskCount;
	std::mutex m_activeTaskCountMutex;
};

#endif // TASK_GROUP_PARALLEL_H
