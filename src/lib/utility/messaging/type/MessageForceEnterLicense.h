#ifndef MESSAGE_FORCE_ENTER_LICENSE_H
#define MESSAGE_FORCE_ENTER_LICENSE_H

#include "utility/messaging/Message.h"

class MessageForceEnterLicense
	: public Message<MessageForceEnterLicense>
{
public:
	MessageForceEnterLicense(bool licenseExpired)
		: licenseExpired(licenseExpired)
	{
		setSendAsTask(false);
	}

	static const std::string getStaticType()
	{
		return "MessageForceEnterLicense";
	}

	bool licenseExpired;
};

#endif // MESSAGE_FORCE_ENTER_LICENSE_H
