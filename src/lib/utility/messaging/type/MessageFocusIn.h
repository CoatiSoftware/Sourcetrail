#ifndef MESSAGE_FOCUS_IN_H
#define MESSAGE_FOCUS_IN_H

#include <vector>

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageFocusIn
	: public Message<MessageFocusIn>
{
public:
	MessageFocusIn(const std::vector<Id>& tokenIds)
		: tokenIds(tokenIds)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageFocusIn";
	}

	const std::vector<Id> tokenIds;
};

#endif //MESSAGE_FOCUS_IN_H
