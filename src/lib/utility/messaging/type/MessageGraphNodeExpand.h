#ifndef MESSAGE_GRAPH_NODE_EXPAND_H
#define MESSAGE_GRAPH_NODE_EXPAND_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageGraphNodeExpand
	: public Message<MessageGraphNodeExpand>
{
public:
	MessageGraphNodeExpand(Id tokenId, bool expand)
		: tokenId(tokenId)
		, expand(expand)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageGraphNodeExpand";
	}

	const Id tokenId;
	const bool expand;
};

#endif // MESSAGE_GRAPH_NODE_EXPAND_H
