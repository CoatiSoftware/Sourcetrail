#ifndef MESSAGE_ACTIVATE_TOKEN_LOCATIONS_H
#define MESSAGE_ACTIVATE_TOKEN_LOCATIONS_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageActivateTokenLocations
	: public Message<MessageActivateTokenLocations>
{
public:
	MessageActivateTokenLocations(const std::vector<Id>& locationIds)
		: locationIds(locationIds)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageActivateTokenLocations";
	}

	const std::vector<Id> locationIds;
};

#endif // MESSAGE_ACTIVATE_TOKEN_LOCATIONS_H
