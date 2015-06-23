#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

#include <memory>
#include <mutex>
#include <deque>

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageInterruptTasks.h"
#include "utility/scheduling/Task.h"

class TaskScheduler
	: public MessageListener<MessageInterruptTasks>
{
public:
	static std::shared_ptr<TaskScheduler> getInstance();

	void pushTask(std::shared_ptr<Task> task);
	void pushNextTask(std::shared_ptr<Task> task);
	void interruptCurrentTask();

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

	void updateTasks();

	virtual void handleMessage(MessageInterruptTasks* message);

	bool m_loopIsRunning;
	bool m_threadIsRunning;

	std::deque<std::shared_ptr<Task>> m_tasks;
	bool m_interruptTask;

	mutable std::mutex m_tasksMutex;
	mutable std::mutex m_loopMutex;
	mutable std::mutex m_threadMutex;
};

#endif // TASK_SCHEDULER_H
