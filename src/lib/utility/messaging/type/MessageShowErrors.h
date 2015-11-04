#ifndef MESSAGE_SHOW_ERRORS_H
#define MESSAGE_SHOW_ERRORS_H

#include "utility/messaging/Message.h"

class MessageShowErrors
	: public Message<MessageShowErrors>
{
public:
	MessageShowErrors()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageShowErrors";
	}
};

#endif // MESSAGE_SHOW_ERRORS_H
