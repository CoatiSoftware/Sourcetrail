#ifndef MESSAGE_TOOLTIP_HIDE_H
#define MESSAGE_TOOLTIP_HIDE_H

#include "utility/messaging/Message.h"

class MessageTooltipHide
	: public Message<MessageTooltipHide>
{
public:
	MessageTooltipHide()
	{
		setSendAsTask(false);
	}

	static const std::string getStaticType()
	{
		return "MessageTooltipHide";
	}
};

#endif // MESSAGE_TOOLTIP_HIDE_H
