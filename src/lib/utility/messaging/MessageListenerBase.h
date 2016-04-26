#ifndef MESSAGE_LISTENER_BASE_H
#define MESSAGE_LISTENER_BASE_H

#include <string>

#include "utility/messaging/MessageBase.h"
#include "utility/messaging/MessageQueue.h"
#include "utility/types.h"

class MessageListenerBase
{
public:
	MessageListenerBase()
		: m_id(s_nextId++)
		, m_alive(true)
	{
		MessageQueue::getInstance()->registerListener(this);
	}

	virtual ~MessageListenerBase()
	{
		if (m_alive)
		{
			m_alive = false;
			MessageQueue::getInstance()->unregisterListener(this);
		}
	}

	uint getId() const
	{
		return m_id;
	}

	std::string getType() const
	{
		if (m_alive)
		{
			return doGetType();
		}
		return "";
	}

	void handleMessageBase(MessageBase* message)
	{
		if (m_alive)
		{
			doHandleMessageBase(message);
		}
	}

	void removedListener()
	{
		m_alive = false;
	}

private:
	virtual std::string doGetType() const = 0;
	virtual void doHandleMessageBase(MessageBase*) = 0;

	static uint s_nextId;

	uint m_id;
	bool m_alive;
};

#endif // MESSAGE_LISTENER_BASE_H
