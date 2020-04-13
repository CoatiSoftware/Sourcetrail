#ifndef MESSAGE_ERROR_COUNT_UPDATE_H
#define MESSAGE_ERROR_COUNT_UPDATE_H

#include "../../Message.h"

#include "../../../../data/ErrorCountInfo.h"

class MessageErrorCountUpdate: public Message<MessageErrorCountUpdate>
{
public:
	static const std::string getStaticType()
	{
		return "MessageErrorCountUpdate";
	}

	MessageErrorCountUpdate(const ErrorCountInfo& errorCount, const std::vector<ErrorInfo>& newErrors)
		: errorCount(errorCount), newErrors(newErrors)
	{
		setSendAsTask(false);
	}

	virtual void print(std::wostream& os) const
	{
		os << errorCount.total << '/' << errorCount.fatal << L" - " << newErrors.size()
		   << L" new errors";
	}

	const ErrorCountInfo errorCount;
	std::vector<ErrorInfo> newErrors;
};

#endif	  // MESSAGE_ERROR_COUNT_UPDATE_H
