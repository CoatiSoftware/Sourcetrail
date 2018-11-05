#ifndef MESSAGE_CODE_SHOW_DEFINITION_H
#define MESSAGE_CODE_SHOW_DEFINITION_H

#include "Message.h"
#include "TabId.h"
#include "types.h"

class MessageCodeShowDefinition
	: public Message<MessageCodeShowDefinition>
{
public:
	static const std::string getStaticType()
	{
		return "MessageCodeShowDefinition";
	}

	MessageCodeShowDefinition(Id nodeId)
		: nodeId(nodeId)
	{
		setSchedulerId(TabId::currentTab());
	}

	virtual void print(std::wostream& os) const
	{
		os << nodeId;
	}

	const Id nodeId;
};

#endif // MESSAGE_CODE_SHOW_DEFINITION_H
