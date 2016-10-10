#include "utility/synchronization/Semaphore.h"

Semaphore::Semaphore(unsigned int counter)
	: m_counter(counter)
{
}

Semaphore::~Semaphore()
{
}

void Semaphore::wait()
{
	std::unique_lock<std::mutex> lock(m_counterMutex);
	while(m_counter == 0)
	{
		m_conditionVariable.wait(lock);
	}
	m_counter--;
}

void Semaphore::post()
{
	std::unique_lock<std::mutex> lock(m_counterMutex);
	m_counter++;
	m_conditionVariable.notify_one();
}
