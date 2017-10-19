#ifndef MESSAGE_FORCE_ENTER_LICENSE_H
#define MESSAGE_FORCE_ENTER_LICENSE_H

#include "LicenseChecker.h"
#include "utility/messaging/Message.h"

class MessageForceEnterLicense
	: public Message<MessageForceEnterLicense>
{
public:
	MessageForceEnterLicense(LicenseChecker::LicenseState state)
		: state(state)
	{
		setSendAsTask(false);
	}

	static const std::string getStaticType()
	{
		return "MessageForceEnterLicense";
	}

	const LicenseChecker::LicenseState state;
};

#endif // MESSAGE_FORCE_ENTER_LICENSE_H
