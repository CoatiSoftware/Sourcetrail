#ifndef MESSAGE_SCROLL_GRAPH_H
#define MESSAGE_SCROLL_GRAPH_H

#include "Message.h"
#include "TabId.h"

class MessageScrollGraph: public Message<MessageScrollGraph>
{
public:
	MessageScrollGraph(int xValue, int yValue): xValue(xValue), yValue(yValue)
	{
		setIsLogged(false);
		setSchedulerId(TabId::currentTab());
	}

	static const std::string getStaticType()
	{
		return "MessageScrollGraph";
	}

	int xValue;
	int yValue;
};

#endif	  // MESSAGE_SCROLL_GRAPH_H
