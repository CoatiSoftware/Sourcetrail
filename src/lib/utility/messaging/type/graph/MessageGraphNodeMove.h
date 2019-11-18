#ifndef MESSAGE_GRAPH_NODE_MOVE_H
#define MESSAGE_GRAPH_NODE_MOVE_H

#include "Message.h"
#include "TabId.h"
#include "Vector2.h"
#include "types.h"

class MessageGraphNodeMove: public Message<MessageGraphNodeMove>
{
public:
	MessageGraphNodeMove(Id tokenId, const Vec2i& delta): tokenId(tokenId), delta(delta)
	{
		setSchedulerId(TabId::currentTab());
	}

	static const std::string getStaticType()
	{
		return "MessageGraphNodeMove";
	}

	virtual void print(std::wostream& os) const
	{
		os << tokenId << L" " << delta.toWString();
	}

	const Id tokenId;
	const Vec2i delta;
};

#endif	  // MESSAGE_GRAPH_NODE_MOVE_H
