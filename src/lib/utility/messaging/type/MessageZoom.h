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

	virtual void print(std::ostream& os) const
	{
		if (zoomIn)
		{
			os << "in";
		}
		else
		{
			os << "out";
		}
	}

	const bool zoomIn;
};

#endif // MESSAGE_ZOOM_H
