#ifndef MESSAGE_SHOW_ERRORS_H
#define MESSAGE_SHOW_ERRORS_H

#include "utility/messaging/Message.h"

class MessageShowErrors
	: public Message<MessageShowErrors>
{
public:
	MessageShowErrors(int errorCount)
		: errorCount(errorCount)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageShowErrors";
	}

	int errorCount;
};

#endif // MESSAGE_SHOW_ERRORS_H
