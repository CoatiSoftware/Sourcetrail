#ifndef MESSAGE_H
#define MESSAGE_H

#include <memory>
#include <string>

#include "utility/messaging/MessageBase.h"
#include "utility/messaging/MessageQueue.h"

template<typename MessageType>
class Message
	: public MessageBase
{
public:
	virtual ~Message() = default;

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

	virtual void print(std::wostream& os) const
	{
	}
};

#endif // MESSAGE_H
