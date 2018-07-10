#ifndef MESSAGE_INDEXING_STARTED_H
#define MESSAGE_INDEXING_STARTED_H

#include "utility/messaging/Message.h"

class MessageIndexingStarted
	: public Message<MessageIndexingStarted>
{
public:
	static const std::string getStaticType()
	{
		return "MessageIndexingStarted";
	}

	MessageIndexingStarted()
	{
	}
};

#endif // MESSAGE_INDEXING_STARTED_H
