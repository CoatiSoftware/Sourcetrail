#ifndef MESSAGE_LISTENER_H
#define MESSAGE_LISTENER_H

#include <string>

#include "utility/messaging/MessageBase.h"
#include "utility/messaging/MessageListenerBase.h"
#include "utility/messaging/MessageQueue.h"

template<typename MessageType>
class MessageListener: public MessageListenerBase
{
public:
	MessageListener(bool toFront = false)
		: MessageListenerBase(toFront)
	{
	}

	virtual std::string getType() const
	{
		return MessageType::getStaticType();
	}

	virtual void handleMessageBase(MessageBase* message)
	{
		handleMessage(dynamic_cast<MessageType*>(message));
	}

private:
	virtual void handleMessage(MessageType* message) = 0;
};

#endif // MESSAGE_LISTENER_H
