#include "TaskDecoratorDelay.h"

#include <thread>

TaskDecoratorDelay::TaskDecoratorDelay(size_t delayMS)
	: m_delayMS(delayMS)
	, m_delayComplete(false)
{
}

void TaskDecoratorDelay::doEnter(std::shared_ptr<Blackboard> blackboard)
{
	m_delayComplete = (m_delayMS == 0);
	m_start = TimeStamp::now();
}

Task::TaskState TaskDecoratorDelay::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	if (m_delayComplete)
	{
		return m_taskRunner->update(blackboard);
	}

	const int SLEEP_TIME_MS = (m_delayMS / 3) + 1;
	std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));

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
