#include "utility/messaging/MessageInterruptTasksCounter.h"

#include "utility/messaging/type/MessageInterruptTasks.h"
#include "utility/messaging/MessageListener.h"

MessageInterruptTasksCounter::MessageInterruptTasksCounter()
	: m_count(0)
{
	class InterruptListener: public MessageListener<MessageInterruptTasks>
	{
	public:
		InterruptListener(size_t& counter)
			: m_counter(counter)
		{}

	private:
		virtual void handleMessage(MessageInterruptTasks* message)
		{
			m_counter++;
		}

		size_t& m_counter;
	};

	m_listener = std::make_shared<InterruptListener>(m_count);
}

MessageInterruptTasksCounter::~MessageInterruptTasksCounter()
{
}

void MessageInterruptTasksCounter::reset()
{
	m_count = 0;
}

size_t MessageInterruptTasksCounter::getCount() const
{
	return m_count;
}
