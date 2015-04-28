#ifndef MESSAGE_GRAPH_NODE_EXPAND_H
#define MESSAGE_GRAPH_NODE_EXPAND_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageGraphNodeExpand
	: public Message<MessageGraphNodeExpand>
{
public:
	MessageGraphNodeExpand(Id tokenId)
		: tokenId(tokenId)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageGraphNodeExpand";
	}

	const Id tokenId;
};

#endif // MESSAGE_GRAPH_NODE_EXPAND_H
