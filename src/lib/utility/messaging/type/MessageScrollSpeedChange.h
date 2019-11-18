#ifndef MESSAGE_SCROLL_SPEED_CHANGE_H
#define MESSAGE_SCROLL_SPEED_CHANGE_H

#include "Message.h"

class MessageScrollSpeedChange: public Message<MessageScrollSpeedChange>
{
public:
	MessageScrollSpeedChange(float scrollSpeed): scrollSpeed(scrollSpeed) {}

	static const std::string getStaticType()
	{
		return "MessageScrollSpeedChange";
	}

	virtual void print(std::wostream& os) const
	{
		os << scrollSpeed;
	}

	const float scrollSpeed;
};

#endif	  // MESSAGE_SCROLL_SPEED_CHANGE_H
