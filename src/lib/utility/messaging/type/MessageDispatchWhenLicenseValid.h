#ifndef MESSAGE_DISPATCH_WHEN_LICENSE_VALID_H
#define MESSAGE_DISPATCH_WHEN_LICENSE_VALID_H

#include "utility/messaging/Message.h"

class MessageDispatchWhenLicenseValid
	: public Message<MessageDispatchWhenLicenseValid>
{
public:
	MessageDispatchWhenLicenseValid(std::shared_ptr<MessageBase> content)
		: content(content)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageDispatchWhenLicenseValid";
	}

	std::shared_ptr<MessageBase> content;
};

#endif // MESSAGE_DISPATCH_WHEN_LICENSE_VALID_H
