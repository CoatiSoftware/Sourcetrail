#ifndef MESSAGE_INDEXING_INTERRUPTED_H
#define MESSAGE_INDEXING_INTERRUPTED_H

#include "../../Message.h"

class MessageIndexingInterrupted: public Message<MessageIndexingInterrupted>
{
public:
	static const std::string getStaticType()
	{
		return "MessageIndexingInterrupted";
	}

	MessageIndexingInterrupted()
	{
		setSendAsTask(false);
	}
};

#endif	  // MESSAGE_INDEXING_INTERRUPTED_H
