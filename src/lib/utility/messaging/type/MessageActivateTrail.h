#ifndef MESSAGE_ACTIVATE_TRAIL_H
#define MESSAGE_ACTIVATE_TRAIL_H

#include "Message.h"
#include "types.h"

class MessageActivateTrail
	: public Message<MessageActivateTrail>
{
public:
	MessageActivateTrail(Id originId, Id targetId, Edge::TypeMask trailType, size_t depth, bool horizontalLayout)
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
	const Edge::TypeMask trailType;
	const size_t depth;
	const bool horizontalLayout;
};

#endif // MESSAGE_ACTIVATE_TRAIL_H
