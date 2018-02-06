#ifndef MESSAGE_FOCUS_IN_H
#define MESSAGE_FOCUS_IN_H

#include <vector>

#include "utility/messaging/Message.h"
#include "utility/types.h"

#include "data/tooltip/TooltipOrigin.h"

class MessageFocusIn
	: public Message<MessageFocusIn>
{
public:
	MessageFocusIn(const std::vector<Id>& tokenIds, TooltipOrigin origin)
		: tokenIds(tokenIds)
		, origin(origin)
	{
		setIsLogged(false);
	}

	static const std::string getStaticType()
	{
		return "MessageFocusIn";
	}

	virtual void print(std::wostream& os) const
	{
		for (const Id& id : tokenIds)
		{
			os << id << L" ";
		}
	}

	const std::vector<Id> tokenIds;
	const TooltipOrigin origin;
};

#endif //MESSAGE_FOCUS_IN_H
