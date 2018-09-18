#ifndef MESSAGE_TOOLTIP_SHOW_H
#define MESSAGE_TOOLTIP_SHOW_H

#include "Message.h"

#include "TooltipOrigin.h"
#include "TooltipInfo.h"

class MessageTooltipShow
	: public Message<MessageTooltipShow>
{
public:
	MessageTooltipShow(TooltipInfo info, TooltipOrigin origin)
		: tooltipInfo(info)
		, origin(origin)
	{
		setSendAsTask(false);
		setIsLogged(false);
	}

	MessageTooltipShow(const std::vector<Id>& sourceLocationIds, const std::vector<Id>& localSymbolIds, TooltipOrigin origin)
		: sourceLocationIds(sourceLocationIds)
		, localSymbolIds(localSymbolIds)
		, origin(origin)
	{
		setSendAsTask(false);
		setIsLogged(false);
	}

	static const std::string getStaticType()
	{
		return "MessageTooltipShow";
	}

	const TooltipInfo tooltipInfo;

	const std::vector<Id> sourceLocationIds;
	const std::vector<Id> localSymbolIds;

	const TooltipOrigin origin;
};

#endif // MESSAGE_TOOLTIP_SHOW_H
