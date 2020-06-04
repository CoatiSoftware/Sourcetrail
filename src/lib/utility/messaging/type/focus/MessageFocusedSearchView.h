#ifndef MESSAGE_FOCUSED_SEARCH_VIEW_H
#define MESSAGE_FOCUSED_SEARCH_VIEW_H

#include "Message.h"
#include "TabId.h"

class MessageFocusedSearchView: public Message<MessageFocusedSearchView>
{
public:
	MessageFocusedSearchView(bool focusIn): focusIn(focusIn)
	{
		setIsLogged(false);
		setSchedulerId(TabId::currentTab());
	}

	static const std::string getStaticType()
	{
		return "MessageFocusedSearchView";
	}

	const bool focusIn;
};

#endif	  // MESSAGE_FOCUSED_SEARCH_VIEW_H
