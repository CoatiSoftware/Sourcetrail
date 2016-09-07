#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

#include <memory>
#include <mutex>
#include <deque>

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageInterruptTasks.h"
#include "utility/scheduling/Task.h"
#include "utility/scheduling/TaskRunner.h"

class TaskScheduler
{
public:
	static std::shared_ptr<TaskScheduler> getInstance();

	void pushTask(std::shared_ptr<Task> task);
	void pushNextTask(std::shared_ptr<Task> task);

	void startSchedulerLoopThreaded();
	void startSchedulerLoop();
	void stopSchedulerLoop();

	bool loopIsRunning() const;
	bool hasTasksQueued() const;

private:
	static std::shared_ptr<TaskScheduler> s_instance;

	TaskScheduler();
	TaskScheduler(const TaskScheduler&);
	void operator=(const TaskScheduler&);

	void processTasks();


	bool m_loopIsRunning;
	bool m_threadIsRunning;

	std::deque<std::shared_ptr<TaskRunner>> m_taskRunners;

	mutable std::mutex m_tasksMutex;
	mutable std::mutex m_loopMutex;
	mutable std::mutex m_threadMutex;
};

#endif // TASK_SCHEDULER_H
