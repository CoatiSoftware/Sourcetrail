#ifndef MESSAGE_ENTERED_LICENSE_H
#define MESSAGE_ENTERED_LICENSE_H

#include "Message.h"

class MessageEnteredLicense
	: public Message<MessageEnteredLicense>
{
public:
	enum LicenseType
	{
		LICENSE_NONE,
		LICENSE_TEST,
		LICENSE_NON_COMMERCIAL,
		LICENSE_COMMERCIAL
	};

	MessageEnteredLicense(LicenseType type)
		: type(type)
	{
		setSendAsTask(false);
	}

	static const std::string getStaticType()
	{
		return "MessageEnteredLicense";
	}

	const LicenseType type;
};

#endif // MESSAGE_ENTERED_LICENSE_H
