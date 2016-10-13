#ifndef MESSAGE_ERROR_FILTER_CHANGED_H
#define MESSAGE_ERROR_FILTER_CHANGED_H

#include "utility/messaging/Message.h"
#include "data/ErrorFilter.h"

class MessageErrorFilterChanged
	: public Message<MessageErrorFilterChanged>
{
public:
	MessageErrorFilterChanged(const ErrorFilter& filter)
		: errorFilter(filter)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageErrorFilterChanged";
	}

	const ErrorFilter errorFilter;
};

#endif // MESSAGE_ERROR_FILTER_CHANGED_H
