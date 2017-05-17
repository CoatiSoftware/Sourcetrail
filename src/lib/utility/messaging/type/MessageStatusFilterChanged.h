#ifndef MESSAGE_STATUS_FILTER_CHANGED_H
#define MESSAGE_STATUS_FILTER_CHANGED_H

#include "utility/messaging/Message.h"
#include "utility/Status.h"

class MessageStatusFilterChanged
	: public Message<MessageStatusFilterChanged>
{
public:
	MessageStatusFilterChanged(const StatusFilter filter)
		: statusFilter(filter)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageStatusFilterChanged";
	}

	const StatusFilter statusFilter;
};

#endif // MESSAGE_STATUS_FILTER_CHANGED_H
