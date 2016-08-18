#ifndef MESSAGE_SCROLL_SPEED_CHANGE_H
#define MESSAGE_SCROLL_SPEED_CHANGE_H

#include "utility/messaging/Message.h"

class MessageScrollSpeedChange
	: public Message<MessageScrollSpeedChange>
{
public:
	MessageScrollSpeedChange(float scrollSpeed)
		: scrollSpeed(scrollSpeed)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageScrollSpeedChange";
	}

	virtual void print(std::ostream& os) const
	{
		os << scrollSpeed;
	}

	const float scrollSpeed;
};

#endif // MESSAGE_SCROLL_SPEED_CHANGE_H
