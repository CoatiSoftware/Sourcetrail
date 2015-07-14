#ifndef MESSAGE_ZOOM_H
#define MESSAGE_ZOOM_H

#include "utility/messaging/Message.h"

class MessageZoom
	: public Message<MessageZoom>
{
public:
	MessageZoom(bool zoomIn)
		: zoomIn(zoomIn)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageZoom";
	}

	const bool zoomIn;
};

#endif // MESSAGE_ZOOM_H
