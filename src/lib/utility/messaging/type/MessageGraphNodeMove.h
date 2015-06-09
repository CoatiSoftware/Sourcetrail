#ifndef MESSAGE_GRAPH_NODE_MOVE_H
#define MESSAGE_GRAPH_NODE_MOVE_H

#include "utility/math/Vector2.h"
#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageGraphNodeMove
	: public Message<MessageGraphNodeMove>
{
public:
	MessageGraphNodeMove(Id tokenId, const Vec2i& delta)
		: tokenId(tokenId)
		, delta(delta)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageGraphNodeMove";
	}

	const Id tokenId;
	const Vec2i delta;
};

#endif // MESSAGE_GRAPH_NODE_MOVE_H
