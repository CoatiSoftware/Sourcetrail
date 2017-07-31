#ifndef MESSAGE_TOOLTIP_SHOW_H
#define MESSAGE_TOOLTIP_SHOW_H

#include "utility/messaging/Message.h"

#include "data/tooltip/TooltipOrigin.h"
#include "data/tooltip/TooltipInfo.h"

class MessageTooltipShow
	: public Message<MessageTooltipShow>
{
public:
	MessageTooltipShow(TooltipInfo info, TooltipOrigin origin)
		: tooltipInfo(info)
		, origin(origin)
	{
		setSendAsTask(false);
	}

	static const std::string getStaticType()
	{
		return "MessageTooltipShow";
	}

	const TooltipInfo tooltipInfo;
	const TooltipOrigin origin;
};

#endif // MESSAGE_TOOLTIP_SHOW_H
