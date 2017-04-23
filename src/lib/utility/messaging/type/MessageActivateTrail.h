#ifndef MESSAGE_ACTIVATE_TRAIL_H
#define MESSAGE_ACTIVATE_TRAIL_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageActivateTrail
	: public Message<MessageActivateTrail>
{
public:
	MessageActivateTrail(Id originId, Id targetId, Edge::EdgeTypeMask trailType, size_t depth, bool horizontalLayout)
		: originId(originId)
		, targetId(targetId)
		, trailType(trailType)
		, depth(depth)
		, horizontalLayout(horizontalLayout)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageActivateTrail";
	}

	const Id originId;
	const Id targetId;
	const Edge::EdgeTypeMask trailType;
	const size_t depth;
	const bool horizontalLayout;
};

#endif // MESSAGE_ACTIVATE_TRAIL_H
