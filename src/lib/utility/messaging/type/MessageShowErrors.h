#ifndef MESSAGE_SHOW_ERRORS_H
#define MESSAGE_SHOW_ERRORS_H

#include "data/ErrorCountInfo.h"
#include "utility/messaging/Message.h"

class MessageShowErrors
	: public Message<MessageShowErrors>
{
public:
	MessageShowErrors(ErrorCountInfo errorCount)
		: errorCount(errorCount)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageShowErrors";
	}

	ErrorCountInfo errorCount;
};

#endif // MESSAGE_SHOW_ERRORS_H
