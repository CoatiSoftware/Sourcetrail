#ifndef MESSAGE_INTERRUPT_TASKS_COUNTER_H
#define MESSAGE_INTERRUPT_TASKS_COUNTER_H

#include <memory>

class MessageListenerBase;

class MessageInterruptTasksCounter
{
public:
	MessageInterruptTasksCounter();
	virtual ~MessageInterruptTasksCounter();

	void reset();
	size_t getCount() const;

private:
	std::shared_ptr<MessageListenerBase> m_listener;

	size_t m_count;
};

#endif // MESSAGE_INTERRUPT_TASKS_COUNTER_H
