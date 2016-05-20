#ifndef TASK_GROUP_PARALLEL_H
#define TASK_GROUP_PARALLEL_H

#include <mutex>
#include <thread>

#include "utility/scheduling/TaskGroup.h"

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

	std::vector<std::thread> m_threads;

	volatile int m_activeTaskCount;
	std::mutex m_activeTaskCountMutex;
};

#endif // TASK_GROUP_PARALLEL_H
