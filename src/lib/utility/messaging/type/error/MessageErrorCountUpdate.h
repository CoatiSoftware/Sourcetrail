#ifndef MESSAGE_ERROR_COUNT_UPDATE_H
#define MESSAGE_ERROR_COUNT_UPDATE_H

#include "utility/messaging/Message.h"

#include "data/ErrorCountInfo.h"

class MessageErrorCountUpdate
	: public Message<MessageErrorCountUpdate>
{
public:
	static const std::string getStaticType()
	{
		return "MessageErrorCountUpdate";
	}

	MessageErrorCountUpdate(const ErrorCountInfo& errorCount)
		: errorCount(errorCount)
	{
		setSendAsTask(false);
	}

	const ErrorCountInfo errorCount;
};

#endif // MESSAGE_ERROR_COUNT_UPDATE_H
