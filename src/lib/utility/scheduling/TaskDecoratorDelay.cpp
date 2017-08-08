#include "utility/scheduling/TaskDecoratorDelay.h"

#include <thread>

TaskDecoratorDelay::TaskDecoratorDelay(size_t delayMS)
	: m_delayMS(delayMS)
	, m_delayComplete(delayMS == 0)
{
}

void TaskDecoratorDelay::doEnter(std::shared_ptr<Blackboard> blackboard)
{
	m_start = TimeStamp::now();
}

Task::TaskState TaskDecoratorDelay::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	if (m_delayComplete)
	{
		return m_taskRunner->update(blackboard);
	}

	const int SLEEP_TIME_MS = 25;
	std::this_thread::sleep_for(std::chrono::microseconds(SLEEP_TIME_MS));

	m_delayComplete = (TimeStamp::now().deltaMS(m_start) >= m_delayMS);

	return Task::STATE_HOLD;
}

void TaskDecoratorDelay::doExit(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskDecoratorDelay::doReset(std::shared_ptr<Blackboard> blackboard)
{
	if (m_delayComplete)
	{
		m_taskRunner->reset();
	}
}

void TaskDecoratorDelay::doTerminate()
{
	if (m_delayComplete)
	{
		m_taskRunner->terminate();
	}
}
