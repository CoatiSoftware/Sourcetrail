#ifndef MESSAGE_RESET_ZOOM_H
#define MESSAGE_RESET_ZOOM_H

#include "../Message.h"

class MessageResetZoom: public Message<MessageResetZoom>
{
public:
	MessageResetZoom() {}

	static const std::string getStaticType()
	{
		return "MessageResetZoom";
	}
};

#endif	  // MESSAGE_RESET_ZOOM_H