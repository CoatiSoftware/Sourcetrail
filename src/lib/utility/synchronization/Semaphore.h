#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <mutex>
#include <condition_variable>

class Semaphore
{
public:
	Semaphore(unsigned int counter);
	~Semaphore();

	void wait();
	void post();

private:
	unsigned int m_counter;
	std::mutex m_counterMutex;
	std::condition_variable m_conditionVariable;
};

#endif // SEMAPHORE_H
