#ifndef MESSAGE_FOCUS_IN_H
#define MESSAGE_FOCUS_IN_H

#include <vector>

#include "Message.h"
#include "TabId.h"
#include "TooltipOrigin.h"
#include "types.h"

class MessageFocusIn: public Message<MessageFocusIn>
{
public:
	MessageFocusIn(const std::vector<Id>& tokenIds, TooltipOrigin origin = TOOLTIP_ORIGIN_NONE)
		: tokenIds(tokenIds), origin(origin)
	{
		setIsLogged(false);
		setSchedulerId(TabId::currentTab());
	}

	static const std::string getStaticType()
	{
		return "MessageFocusIn";
	}

	virtual void print(std::wostream& os) const
	{
		for (const Id& id: tokenIds)
		{
			os << id << L" ";
		}
	}

	const std::vector<Id> tokenIds;
	const TooltipOrigin origin;
};

#endif	  // MESSAGE_FOCUS_IN_H
