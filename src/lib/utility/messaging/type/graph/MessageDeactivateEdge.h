#ifndef MESSAGE_DEACTIVATE_EDGE_H
#define MESSAGE_DEACTIVATE_EDGE_H

#include "../../Message.h"
#include "../../../../component/TabId.h"

class MessageDeactivateEdge: public Message<MessageDeactivateEdge>
{
public:
	MessageDeactivateEdge(bool scrollToDefinition): scrollToDefinition(scrollToDefinition)
	{
		setSchedulerId(TabId::currentTab());
	}

	static const std::string getStaticType()
	{
		return "MessageDeactivateEdge";
	}

	bool scrollToDefinition;
};

#endif	  // MESSAGE_DEACTIVATE_EDGE_H
