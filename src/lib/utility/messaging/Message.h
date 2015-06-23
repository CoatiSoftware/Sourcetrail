#ifndef MESSAGE_H
#define MESSAGE_H

#include <memory>
#include <string>

#include "utility/messaging/MessageBase.h"
#include "utility/messaging/MessageQueue.h"

template<typename MessageType>
class Message: public MessageBase
{
public:

	virtual ~Message()
	{
	}

	virtual std::string getType() const
	{
		return MessageType::getStaticType();
	}

	virtual void dispatch()
	{
		std::shared_ptr<MessageBase> message = std::make_shared<MessageType>(*dynamic_cast<MessageType*>(this));
		MessageQueue::getInstance()->pushMessage(message);
	}

	virtual void dispatchImmediately()
	{
		std::shared_ptr<MessageBase> message = std::make_shared<MessageType>(*dynamic_cast<MessageType*>(this));
		MessageQueue::getInstance()->processMessage(message, true);
	}
};

#endif // MESSAGE_H
