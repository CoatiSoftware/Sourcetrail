#ifndef MESSAGE_ACTIVATE_NODES_H
#define MESSAGE_ACTIVATE_NODES_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

#include "data/graph/Node.h"

class MessageActivateNodes
	: public Message<MessageActivateNodes>
{
public:
	struct ActiveNode
	{
		Id nodeId;
		Node::NodeType type;
		std::string name;
	};

	MessageActivateNodes()
		: isFromSystem(false)
	{
	}

	void addNode(Id tokenId, Node::NodeType type, const std::string& name)
	{
		ActiveNode node;
		node.nodeId = tokenId;
		node.type = type;
		node.name = name;

		nodes.push_back(node);
	}

	static const std::string getStaticType()
	{
		return "MessageActivateNodes";
	}

	std::vector<ActiveNode> nodes;

	bool isFromSystem;
};

#endif // MESSAGE_ACTIVATE_NODES_H
