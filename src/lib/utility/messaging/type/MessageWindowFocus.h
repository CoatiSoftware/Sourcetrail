#ifndef MESSAGE_WINDOW_FOCUS_H
#define MESSAGE_WINDOW_FOCUS_H

#include "utility/messaging/Message.h"

class MessageWindowFocus: public Message<MessageWindowFocus>
{
public:
	MessageWindowFocus()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageWindowFocus";
	}
};

#endif // MESSAGE_WINDOW_FOCUS_H
