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
		NameHierarchy nameHierarchy;
	};

	MessageActivateNodes()
	{
	}

	void addNode(Id tokenId, Node::NodeType type, const NameHierarchy& nameHierarchy)
	{
		ActiveNode node;
		node.nodeId = tokenId;
		node.type = type;
		node.nameHierarchy = nameHierarchy;

		nodes.push_back(node);
	}

	static const std::string getStaticType()
	{
		return "MessageActivateNodes";
	}

	virtual void print(std::ostream& os) const
	{
		for (const ActiveNode& node : nodes)
		{
			os << node.nodeId << " ";
		}
	}

	std::vector<ActiveNode> nodes;
};

#endif // MESSAGE_ACTIVATE_NODES_H
