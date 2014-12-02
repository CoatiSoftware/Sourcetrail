#ifndef MESSAGE_LISTENER_BASE_H
#define MESSAGE_LISTENER_BASE_H

#include <string>

#include "utility/messaging/MessageBase.h"
#include "utility/messaging/MessageQueue.h"

class MessageListenerBase
{
public:
	MessageListenerBase(bool toFront)
	{
		MessageQueue::getInstance()->registerListener(this, toFront);
	}

	virtual ~MessageListenerBase()
	{
		MessageQueue::getInstance()->unregisterListener(this);
	}

	virtual std::string getType() const = 0;

	virtual void handleMessageBase(MessageBase*) = 0;
};

#endif // MESSAGE_LISTENER_BASE_H
