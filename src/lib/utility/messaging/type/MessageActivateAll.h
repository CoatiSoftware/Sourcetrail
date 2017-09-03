#ifndef MESSAGE_ACTIVATE_ALL_H
#define MESSAGE_ACTIVATE_ALL_H

#include "data/graph/Node.h"
#include "utility/messaging/Message.h"

class MessageActivateAll
	: public Message<MessageActivateAll>
{
public:
	MessageActivateAll(Node::NodeTypeMask filter = 0)
		: filter(filter)
	{
		setIsParallel(true);
	}

	static const std::string getStaticType()
	{
		return "MessageActivateAll";
	}

	Node::NodeTypeMask filter;
};

#endif // MESSAGE_ACTIVATE_ALL_H
