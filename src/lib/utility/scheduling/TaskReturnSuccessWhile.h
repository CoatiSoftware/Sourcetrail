#ifndef TASK_RETURN_SUCCESS_WHILE_H
#define TASK_RETURN_SUCCESS_WHILE_H

#include <vector>

#include "utility/scheduling/Task.h"
#include "utility/scheduling/Blackboard.h"

template <typename T>
class TaskReturnSuccessWhile:
	public Task
{
public:
	enum ConditionType
	{
		CONDITION_GREATER_THAN,
		CONDITION_EQUALS
	};

	TaskReturnSuccessWhile(const std::string& lhsValueName, ConditionType condition, T rhsValue);

private:
	virtual void doEnter(std::shared_ptr<Blackboard> blackboard);
	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blackboard);
	virtual void doExit(std::shared_ptr<Blackboard> blackboard);
	virtual void doReset(std::shared_ptr<Blackboard> blackboard);

	const std::string m_lhsValueName;
	const ConditionType m_condition;
	const T m_rhsValue;
};

template <typename T>
TaskReturnSuccessWhile<T>::TaskReturnSuccessWhile(const std::string& lhsValueName, ConditionType condition, T rhsValue)
	: m_lhsValueName(lhsValueName)
	, m_condition(condition)
	, m_rhsValue(rhsValue)
{
}

template <typename T>
void TaskReturnSuccessWhile<T>::doEnter(std::shared_ptr<Blackboard> blackboard)
{
}

template <typename T>
Task::TaskState TaskReturnSuccessWhile<T>::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	const int SLEEP_TIME_MS = 25;
	std::this_thread::sleep_for(std::chrono::microseconds(SLEEP_TIME_MS));

	T lhsValue = 0;
	blackboard->get<T>(m_lhsValueName, lhsValue);

	switch (m_condition)
	{
	case CONDITION_GREATER_THAN:
		if (lhsValue > m_rhsValue)
		{
			return STATE_SUCCESS;
		}
		break;
	}

	return STATE_FAILURE;
}

template <typename T>
void TaskReturnSuccessWhile<T>::doExit(std::shared_ptr<Blackboard> blackboard)
{
}

template <typename T>
void TaskReturnSuccessWhile<T>::doReset(std::shared_ptr<Blackboard> blackboard)
{
}

#endif // TASK_RETURN_SUCCESS_WHILE_H
