#ifndef MESSAGE_ACTIVATE_ALL_H
#define MESSAGE_ACTIVATE_ALL_H

#include "data/graph/Node.h"
#include "data/NodeTypeSet.h"
#include "utility/messaging/Message.h"

class MessageActivateAll
	: public Message<MessageActivateAll>
{
public:
	MessageActivateAll(NodeTypeSet acceptedNodeTypes = NodeTypeSet::all())
		: acceptedNodeTypes(acceptedNodeTypes)
	{
		setIsParallel(true);
	}

	static const std::string getStaticType()
	{
		return "MessageActivateAll";
	}

	NodeTypeSet acceptedNodeTypes;
};

#endif // MESSAGE_ACTIVATE_ALL_H
