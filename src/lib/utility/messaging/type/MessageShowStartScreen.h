#ifndef MESSAGE_SHOW_START_SCREEN_H
#define MESSAGE_SHOW_START_SCREEN_H

#include "utility/messaging/Message.h"

class MessageShowStartScreen
	: public Message<MessageShowStartScreen>
{
public:
	MessageShowStartScreen()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageShowStartScreen";
	}
};

#endif // MESSAGE_SHOW_START_SCREEN_H
