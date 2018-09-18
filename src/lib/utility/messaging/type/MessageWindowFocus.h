#ifndef MESSAGE_WINDOW_FOCUS_H
#define MESSAGE_WINDOW_FOCUS_H

#include "Message.h"

class MessageWindowFocus
	: public Message<MessageWindowFocus>
{
public:
	MessageWindowFocus(bool focusIn)
		: focusIn(focusIn)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageWindowFocus";
	}

	const bool focusIn;
};

#endif // MESSAGE_WINDOW_FOCUS_H
