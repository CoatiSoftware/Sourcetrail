#ifndef MESSAGE_ACTIVATE_TOKEN_LOCATION_H
#define MESSAGE_ACTIVATE_TOKEN_LOCATION_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageActivateTokenLocation: public Message<MessageActivateTokenLocation>
{
public:
	MessageActivateTokenLocation(Id locationId)
		: locationId(locationId)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageActivateTokenLocation";
	}

	const Id locationId;
};

#endif // MESSAGE_ACTIVATE_TOKEN_LOCATION_H
