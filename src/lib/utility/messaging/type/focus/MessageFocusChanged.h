#ifndef MESSAGE_FOCUS_CHANGED_H
#define MESSAGE_FOCUS_CHANGED_H

#include "Message.h"
#include "TabId.h"

class MessageFocusChanged: public Message<MessageFocusChanged>
{
public:
	enum class ViewType
	{
		GRAPH,
		CODE
	};

	MessageFocusChanged(ViewType type, Id tokenOrLocationId)
		: type(type), tokenOrLocationId(tokenOrLocationId)
	{
		setIsLogged(false);
		setSchedulerId(TabId::currentTab());
	}

	static const std::string getStaticType()
	{
		return "MessageFocusChanged";
	}

	virtual void print(std::wostream& os) const
	{
		switch (type)
		{
		case ViewType::GRAPH:
			os << "graph";
			break;
		case ViewType::CODE:
			os << "code";
			break;
		}

		os << " " << tokenOrLocationId;
	}

	bool isFromGraph() const
	{
		return type == ViewType::GRAPH;
	}

	bool isFromCode() const
	{
		return type == ViewType::CODE;
	}

	const ViewType type;
	const Id tokenOrLocationId;
};

#endif	  // MESSAGE_FOCUS_CHANGED_H
