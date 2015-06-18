#ifndef MESSAGE_ACTIVATE_EDGE_H
#define MESSAGE_ACTIVATE_EDGE_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

#include "data/graph/Edge.h"

class MessageActivateEdge
	: public Message<MessageActivateEdge>
{
public:
	MessageActivateEdge(Id tokenId, Edge::EdgeType type, const std::string& name)
		: tokenId(tokenId)
		, type(type)
		, name(name)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageActivateEdge";
	}

	const Id tokenId;
	const Edge::EdgeType type;
	const std::string name;
};

#endif // MESSAGE_ACTIVATE_EDGE_H
