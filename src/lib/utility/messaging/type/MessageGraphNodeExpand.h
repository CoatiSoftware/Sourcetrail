#ifndef MESSAGE_GRAPH_NODE_EXPAND_H
#define MESSAGE_GRAPH_NODE_EXPAND_H

#include "utility/messaging/Message.h"
#include "utility/types.h"
#include "data/graph/token_component/TokenComponentAccess.h"

class MessageGraphNodeExpand: public Message<MessageGraphNodeExpand>
{
public:
	MessageGraphNodeExpand(Id tokenId, TokenComponentAccess::AccessType access)
		: tokenId(tokenId)
		, access(access)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageGraphNodeExpand";
	}

	const Id tokenId;
	const TokenComponentAccess::AccessType access;
};

#endif // MESSAGE_GRAPH_NODE_EXPAND_H
