#ifndef TASK_RETURN_SUCCESS_IF_H
#define TASK_RETURN_SUCCESS_IF_H

#include "utility/scheduling/Task.h"
#include "utility/scheduling/Blackboard.h"

template <typename T>
class TaskReturnSuccessIf:
	public Task
{
public:
	enum ConditionType
	{
		CONDITION_GREATER_THAN,
		CONDITION_EQUALS
	};

	TaskReturnSuccessIf(const std::string& lhsValueName, ConditionType condition, T rhsValue);

private:
	void doEnter(std::shared_ptr<Blackboard> blackboard) override;
	TaskState doUpdate(std::shared_ptr<Blackboard> blackboard) override;
	void doExit(std::shared_ptr<Blackboard> blackboard) override;
	void doReset(std::shared_ptr<Blackboard> blackboard) override;

	const std::string m_lhsValueName;
	const ConditionType m_condition;
	const T m_rhsValue;
};

template <typename T>
TaskReturnSuccessIf<T>::TaskReturnSuccessIf(const std::string& lhsValueName, ConditionType condition, T rhsValue)
	: m_lhsValueName(lhsValueName)
	, m_condition(condition)
	, m_rhsValue(rhsValue)
{
}

template <typename T>
void TaskReturnSuccessIf<T>::doEnter(std::shared_ptr<Blackboard> blackboard)
{
}

template <typename T>
Task::TaskState TaskReturnSuccessIf<T>::doUpdate(std::shared_ptr<Blackboard> blackboard)
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
	case CONDITION_EQUALS:
		if (lhsValue == m_rhsValue)
		{
			return STATE_SUCCESS;
		}
		break;
	}
	return STATE_FAILURE;
}

template <typename T>
void TaskReturnSuccessIf<T>::doExit(std::shared_ptr<Blackboard> blackboard)
{
}

template <typename T>
void TaskReturnSuccessIf<T>::doReset(std::shared_ptr<Blackboard> blackboard)
{
}

#endif // TASK_RETURN_SUCCESS_IF_H
