#ifndef MESSAGE_STATUS_H
#define MESSAGE_STATUS_H

#include "utility/messaging/Message.h"
#include "utility/utilityString.h"

class MessageStatus
	: public Message<MessageStatus>
{
public:
	MessageStatus(const std::string& status, bool isError = false, bool showLoader = false)
		: status(utility::replace(status, "\n", " "))
		, isError(isError)
		, showLoader(showLoader)
	{
		setSendAsTask(false);
	}

	static const std::string getStaticType()
	{
		return "MessageStatus";
	}

	virtual void print(std::ostream& os) const
	{
		os << status;

		if (isError)
		{
			os << " - error";
		}

		if (showLoader)
		{
			os << " - loading";
		}
	}

	const std::string status;
	const bool isError;
	const bool showLoader;
};

#endif // MESSAGE_STATUS_H
