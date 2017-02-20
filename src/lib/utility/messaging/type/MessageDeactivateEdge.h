#ifndef MESSAGE_DEACTIVATE_EDGE_H
#define MESSAGE_DEACTIVATE_EDGE_H

#include "utility/messaging/Message.h"

class MessageDeactivateEdge
	: public Message<MessageDeactivateEdge>
{
public:
	MessageDeactivateEdge(bool scrollToDefinition)
		: scrollToDefinition(scrollToDefinition)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageDeactivateEdge";
	}

	bool scrollToDefinition;
};

#endif // MESSAGE_DEACTIVATE_EDGE_H
