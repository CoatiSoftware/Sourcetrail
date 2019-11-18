#ifndef MESSAGE_ACTIVATE_SOURCE_LOCATIONS_H
#define MESSAGE_ACTIVATE_SOURCE_LOCATIONS_H

#include "Message.h"
#include "TabId.h"
#include "types.h"

class MessageActivateSourceLocations: public Message<MessageActivateSourceLocations>
{
public:
	MessageActivateSourceLocations(const std::vector<Id>& locationIds, bool containsUnsolvedLocations)
		: locationIds(locationIds), containsUnsolvedLocations(containsUnsolvedLocations)
	{
		setSchedulerId(TabId::currentTab());
	}

	static const std::string getStaticType()
	{
		return "MessageActivateSourceLocations";
	}

	virtual void print(std::wostream& os) const
	{
		for (const Id& id: locationIds)
		{
			os << id << L" ";
		}
	}

	const std::vector<Id> locationIds;
	const bool containsUnsolvedLocations;
};

#endif	  // MESSAGE_ACTIVATE_SOURCE_LOCATIONS_H
