#ifndef MESSAGE_ACTIVATE_LEGEND_H
#define MESSAGE_ACTIVATE_LEGEND_H

#include "utility/messaging/Message.h"
#include "utility/messaging/type/MessageActivateBase.h"

class MessageActivateLegend
	: public Message<MessageActivateLegend>
	, public MessageActivateBase
{
public:
	static const std::string getStaticType()
	{
		return "MessageActivateLegend";
	}

	std::vector<SearchMatch> getSearchMatches() const override
	{
		return { SearchMatch::createCommand(SearchMatch::COMMAND_LEGEND) };
	}
};

#endif // MESSAGE_ACTIVATE_LEGEND_H
