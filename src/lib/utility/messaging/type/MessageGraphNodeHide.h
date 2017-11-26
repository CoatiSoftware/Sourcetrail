#ifndef MESSAGE_GRAPH_NODE_HIDE_H
#define MESSAGE_GRAPH_NODE_HIDE_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageGraphNodeHide
	: public Message<MessageGraphNodeHide>
{
public:
	MessageGraphNodeHide(Id tokenId)
		: tokenId(tokenId)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageGraphNodeHide";
	}

	virtual void print(std::ostream& os) const
	{
		os << tokenId;
	}

	const Id tokenId;
};

#endif // MESSAGE_GRAPH_NODE_HIDE_H
