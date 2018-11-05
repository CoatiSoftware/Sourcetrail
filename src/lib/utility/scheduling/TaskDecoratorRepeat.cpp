#include "TaskDecoratorRepeat.h"

#include <chrono>
#include <thread>

TaskDecoratorRepeat::TaskDecoratorRepeat(ConditionType condition, TaskState exitState, size_t delayMS)
	: m_condition(condition)
	, m_exitState(exitState)
	, m_delayMS(delayMS)
{
}

void TaskDecoratorRepeat::doEnter(std::shared_ptr<Blackboard> blackboard)
{
}

Task::TaskState TaskDecoratorRepeat::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	TaskState state = m_taskRunner->update(blackboard);

	switch (m_condition)
	{
	case CONDITION_WHILE_SUCCESS:
		if (state == Task::STATE_SUCCESS)
		{
			m_taskRunner->reset();
			state = Task::STATE_RUNNING;
		}
		else if (state == Task::STATE_FAILURE)
		{
			return m_exitState;
		}
		break;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(m_delayMS));

	return state;
}

void TaskDecoratorRepeat::doExit(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskDecoratorRepeat::doReset(std::shared_ptr<Blackboard> blackboard)
{
	m_taskRunner->reset();
}
