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
	}

	static const std::string getStaticType()
	{
		return "MessageHoverLeave";
	}

	const std::vector<Id> tokenIds;
};

#endif //MESSAGE_FOCUS_OUT_H
