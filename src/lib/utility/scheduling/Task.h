#ifndef TASK_H
#define TASK_H

#include <memory>

class Task
{
public:
	enum TaskState
	{
		STATE_NEW,
		STATE_RUNNING,
		STATE_FINISHED,
		STATE_CANCELED
	};

	static void dispatch(std::shared_ptr<Task> task);
	static void dispatchNext(std::shared_ptr<Task> task);

	Task();
	virtual ~Task();

	TaskState getState() const;

	TaskState processTask();
	TaskState interruptTask();

	void executeTask();

	virtual void enter() = 0;
	virtual TaskState update() = 0;
	virtual void exit() = 0;

	virtual void interrupt() = 0;
	virtual void revert() = 0;

protected:
	void setState(TaskState state);

private:
	TaskState m_state;
};

#endif // TASK_H
