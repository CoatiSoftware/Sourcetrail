#ifndef MESSAGE_ACTIVATE_SOURCE_LOCATIONS_H
#define MESSAGE_ACTIVATE_SOURCE_LOCATIONS_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageActivateSourceLocations
	: public Message<MessageActivateSourceLocations>
{
public:
	MessageActivateSourceLocations(const std::vector<Id>& locationIds)
		: locationIds(locationIds)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageActivateSourceLocations";
	}

	virtual void print(std::wostream& os) const
	{
		for (const Id& id : locationIds)
		{
			os << id << L" ";
		}
	}

	const std::vector<Id> locationIds;
};

#endif // MESSAGE_ACTIVATE_SOURCE_LOCATIONS_H
