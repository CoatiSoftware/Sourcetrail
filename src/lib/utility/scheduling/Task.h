#ifndef TASK_H
#define TASK_H

#include <memory>

class Blackboard;

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

	TaskState update(std::shared_ptr<Blackboard> blackboard);
	void reset(std::shared_ptr<Blackboard> blackboard);

private:
	virtual void doEnter(std::shared_ptr<Blackboard> blackboard) = 0;
	virtual Task::TaskState doUpdate(std::shared_ptr<Blackboard> blackboard) = 0;
	virtual void doExit(std::shared_ptr<Blackboard> blackboard) = 0;
	virtual void doReset(std::shared_ptr<Blackboard> blackboard) = 0;

	bool m_enterCalled;
	bool m_exitCalled;
};

#endif // TASK_H
