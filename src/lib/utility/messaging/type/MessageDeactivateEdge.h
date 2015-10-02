#ifndef MESSAGE_DEACTIVATE_EDGE_H
#define MESSAGE_DEACTIVATE_EDGE_H

#include "utility/messaging/Message.h"

class MessageDeactivateEdge
	: public Message<MessageDeactivateEdge>
{
public:
	MessageDeactivateEdge()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageDeactivateEdge";
	}
};

#endif // MESSAGE_DEACTIVATE_EDGE_H
