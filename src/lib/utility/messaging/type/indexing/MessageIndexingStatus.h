#ifndef MESSAGE_INDEXING_STATUS_H
#define MESSAGE_INDEXING_STATUS_H

#include "utility/messaging/Message.h"

class MessageIndexingStatus
	: public Message<MessageIndexingStatus>
{
public:
	static const std::string getStaticType()
	{
		return "MessageIndexingStatus";
	}

	MessageIndexingStatus(bool showProgress, bool unknownProgress, size_t progressPercent)
		: showProgress(showProgress)
		, unknownProgress(unknownProgress)
		, progressPercent(progressPercent)
	{
		setSendAsTask(false);
	}

	const bool showProgress;
	const bool unknownProgress;
	const size_t progressPercent;
};

#endif // MESSAGE_INDEXING_STATUS_H
