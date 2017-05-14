#ifndef MESSAGE_FILTER_SEARCH_AUTOCOMPLETE_H
#define MESSAGE_FILTER_SEARCH_AUTOCOMPLETE_H

#include "utility/messaging/MessageFilter.h"
#include "utility/messaging/type/MessageSearchAutocomplete.h"

class MessageFilterSearchAutocomplete
	: public MessageFilter
{
	void filter(MessageQueue::MessageBufferType* messageBuffer) override
	{
		MessageBase* message = messageBuffer->front().get();
		if (message->getType() == MessageSearchAutocomplete::getStaticType())
		{
			for (auto it = messageBuffer->begin() + 1; it != messageBuffer->end(); it++)
			{
				if ((*it)->getType() == MessageSearchAutocomplete::getStaticType())
				{
					messageBuffer->pop_front();
					return;
				}
			}
		}
	}
};

#endif // MESSAGE_FILTER_SEARCH_AUTOCOMPLETE_H
