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
	{
		MessageQueue::getInstance()->registerListener(this);
	}

	virtual ~MessageListenerBase()
	{
		MessageQueue::getInstance()->unregisterListener(this);
	}

	uint getId() const
	{
		return m_id;
	}

	virtual std::string getType() const = 0;

	virtual void handleMessageBase(MessageBase*) = 0;

private:
	static uint s_nextId;

	uint m_id;
};

#endif // MESSAGE_LISTENER_BASE_H
