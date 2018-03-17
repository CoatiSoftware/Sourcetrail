#ifndef MESSAGE_SHOW_ERRORS_H
#define MESSAGE_SHOW_ERRORS_H

#include "utility/messaging/Message.h"

#include "data/ErrorCountInfo.h"
#include "data/ErrorInfo.h"

class MessageShowErrors
	: public Message<MessageShowErrors>
{
public:
	MessageShowErrors(ErrorCountInfo errorCount)
		: errorCount(errorCount)
		, errorId(0)
	{
	}

	MessageShowErrors(const std::vector<ErrorInfo>& errors)
		: errors(errors)
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

	const ErrorCountInfo errorCount;
	const std::vector<ErrorInfo> errors;
	const Id errorId;
};

#endif // MESSAGE_SHOW_ERRORS_H
