#ifndef TASK_H
#define TASK_H

#include <memory>

class Task
{
public:
	enum TaskState
	{
		STATE_RUNNING,
		STATE_SUCCESS,
		STATE_FAILURE
	};

	static void dispatch(std::shared_ptr<Task> task);
	static void dispatchNext(std::shared_ptr<Task> task);

	Task();
	virtual ~Task();

//	virtual TaskState getState() const = 0;

	TaskState update();
	void reset();

private:
	virtual void doEnter() = 0;
	virtual Task::TaskState doUpdate() = 0;
	virtual void doExit() = 0;
	virtual void doReset() = 0;

	bool m_enterCalled;
	bool m_exitCalled;
};

#endif // TASK_H
