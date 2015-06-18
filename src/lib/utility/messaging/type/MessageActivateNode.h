#ifndef MESSAGE_ACTIVATE_NODE_H
#define MESSAGE_ACTIVATE_NODE_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

#include "data/graph/Node.h"

class MessageActivateNode
	: public Message<MessageActivateNode>
{
public:
	MessageActivateNode(Id tokenId, Node::NodeType type, const std::string& name)
		: tokenId(tokenId)
		, type(type)
		, name(name)
		, isFromSystem(false)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageActivateNode";
	}

	const Id tokenId;
	const Node::NodeType type;
	const std::string name;

	bool isFromSystem;
};

#endif // MESSAGE_ACTIVATE_NODE_H
