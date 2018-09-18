#ifndef TASK_SET_VALUE_H
#define TASK_SET_VALUE_H

#include "Task.h"
#include "Blackboard.h"

template <typename T>
class TaskSetValue:
	public Task
{
public:
	TaskSetValue(const std::string& valueName, T value);

private:
	void doEnter(std::shared_ptr<Blackboard> blackboard) override;
	TaskState doUpdate(std::shared_ptr<Blackboard> blackboard) override;
	void doExit(std::shared_ptr<Blackboard> blackboard) override;
	void doReset(std::shared_ptr<Blackboard> blackboard) override;

	const std::string m_valueName;
	const T m_value;
};

template <typename T>
TaskSetValue<T>::TaskSetValue(const std::string& valueName, T value)
	: m_valueName(valueName)
	, m_value(value)
{
}

template <typename T>
void TaskSetValue<T>::doEnter(std::shared_ptr<Blackboard> blackboard)
{
}

template <typename T>
Task::TaskState TaskSetValue<T>::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	blackboard->set<T>(m_valueName, m_value);
	return STATE_SUCCESS;
}

template <typename T>
void TaskSetValue<T>::doExit(std::shared_ptr<Blackboard> blackboard)
{
}

template <typename T>
void TaskSetValue<T>::doReset(std::shared_ptr<Blackboard> blackboard)
{
}

#endif // TASK_SET_VALUE_H
