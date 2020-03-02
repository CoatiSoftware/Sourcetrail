#ifndef MESSAGE_REFRESH_UI_STATE_H
#define MESSAGE_REFRESH_UI_STATE_H

#include "Message.h"

class MessageRefreshUIState: public Message<MessageRefreshUIState>
{
public:
	static const std::string getStaticType()
	{
		return "MessageRefreshUIState";
	}

	MessageRefreshUIState(bool isAfterIndexing) : isAfterIndexing(isAfterIndexing) {}

	bool isAfterIndexing = false;
};

#endif	  // MESSAGE_REFRESH_UI_STATE_H
