#ifndef MESSAGE_FOCUS_OUT_H
#define MESSAGE_FOCUS_OUT_H

#include <vector>

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageFocusOut
	: public Message<MessageFocusOut>
{
public:
	MessageFocusOut(const std::vector<Id>& tokenIds)
		: tokenIds(tokenIds)
	{
		setIsLogged(false);
	}

	static const std::string getStaticType()
	{
		return "MessageFocusOut";
	}

	virtual void print(std::ostream& os) const
	{
		for (const Id& id : tokenIds)
		{
			os << id << " ";
		}
	}

	const std::vector<Id> tokenIds;
};

#endif //MESSAGE_FOCUS_OUT_H
