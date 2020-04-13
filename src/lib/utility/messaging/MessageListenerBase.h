#ifndef MESSAGE_LISTENER_BASE_H
#define MESSAGE_LISTENER_BASE_H

#include <string>

#include "MessageBase.h"
#include "MessageQueue.h"
#include "../types.h"

class MessageListenerBase
{
public:
	MessageListenerBase(): m_id(s_nextId++), m_alive(true)
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

	Id getId() const
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

	virtual Id getSchedulerId() const
	{
		return 0;
	}

private:
	virtual std::string doGetType() const = 0;
	virtual void doHandleMessageBase(MessageBase*) = 0;

	static Id s_nextId;

	Id m_id;
	bool m_alive;
};

#endif	  // MESSAGE_LISTENER_BASE_H
