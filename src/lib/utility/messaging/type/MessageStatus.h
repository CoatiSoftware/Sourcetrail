#ifndef MESSAGE_STATUS_H
#define MESSAGE_STATUS_H

#include "utility/messaging/Message.h"

class MessageStatus
	: public Message<MessageStatus>
{
public:
	MessageStatus(const std::string& status, bool isError = false, bool showLoader = false)
		: status(status)
		, isError(isError)
		, showLoader(showLoader)
	{
		setSendAsTask(false);
	}

	static const std::string getStaticType()
	{
		return "MessageStatus";
	}

	const std::string status;
	const bool isError;
	const bool showLoader;
};

#endif // MESSAGE_STATUS_H
