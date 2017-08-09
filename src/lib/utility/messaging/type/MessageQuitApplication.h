#ifndef MESSAGE_QUIT_APPLICATION_H
#define MESSAGE_QUIT_APPLICATION_H

#include "utility/messaging/Message.h"

class MessageQuitApplication
	: public Message<MessageQuitApplication>
{
public:
	MessageQuitApplication()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageQuitApplication";
	}
};

#endif // MESSAGE_QUIT_APPLICATION_H
