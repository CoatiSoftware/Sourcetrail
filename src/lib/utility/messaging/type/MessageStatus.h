#ifndef MESSAGE_STATUS_H
#define MESSAGE_STATUS_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageStatus: public Message<MessageStatus>
{
public:
	MessageStatus(
		const std::string& status
	)
		: status(status)

	{
	}

	static const std::string getStaticType()
	{
		return "MessageStatus";
	}

	const std::string status;
};

#endif // MESSAGE_STATUS_H
