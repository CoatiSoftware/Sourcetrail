#ifndef MESSAGE_ENTERED_LICENSE_H
#define MESSAGE_ENTERED_LICENSE_H

#include "utility/messaging/Message.h"

class MessageEnteredLicense
	: public Message<MessageEnteredLicense>
{
public:
	MessageEnteredLicense()
	{
		setSendAsTask(false);
	}

	static const std::string getStaticType()
	{
		return "MessageEnteredLicense";
	}
};

#endif // MESSAGE_ENTERED_LICENSE_H
