#ifndef MESSAGE_LISTENER_H
#define MESSAGE_LISTENER_H

#include <string>

#include "MessageBase.h"
#include "MessageListenerBase.h"
#include "MessageQueue.h"

template <typename MessageType>
class MessageListener: public MessageListenerBase
{
public:
	MessageListener() {}

private:
	virtual std::string doGetType() const
	{
		return MessageType::getStaticType();
	}

	virtual void doHandleMessageBase(MessageBase* message)
	{
		// if (message->isLogged())
		// {
		// 	LOG_INFO_STREAM_BARE(<< "handle " << message->str());
		// }

		handleMessage(dynamic_cast<MessageType*>(message));
	}

	virtual void handleMessage(MessageType* message) = 0;
};

#endif	  // MESSAGE_LISTENER_H
