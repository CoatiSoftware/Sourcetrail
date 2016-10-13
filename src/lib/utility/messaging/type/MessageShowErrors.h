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
		, errorId(0)
	{
	}

	MessageShowErrors(Id errorId)
		: errorId(errorId)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageShowErrors";
	}

	ErrorCountInfo errorCount;
	Id errorId;
};

#endif // MESSAGE_SHOW_ERRORS_H
