#ifndef MESSAGE_ACTIVATE_ERRORS_H
#define MESSAGE_ACTIVATE_ERRORS_H

#include "data/ErrorFilter.h"
#include "utility/messaging/Message.h"

class MessageActivateErrors
	: public Message<MessageActivateErrors>
{
public:
	static const std::string getStaticType()
	{
		return "MessageActivateErrors";
	}

	MessageActivateErrors(const ErrorFilter& filter, const FilePath& file = FilePath())
		: filter(filter)
		, file(file)
	{
	}

	const ErrorFilter filter;
	const FilePath file;
};

#endif // MESSAGE_ACTIVATE_ERRORS_H
