#ifndef MESSAGE_FILTER_ERROR_COUNT_UPDATE_H
#define MESSAGE_FILTER_ERROR_COUNT_UPDATE_H

#include "utility/messaging/MessageFilter.h"
#include "utility/messaging/type/error/MessageErrorCountUpdate.h"

class MessageFilterErrorCountUpdate
	: public MessageFilter
{
	void filter(MessageQueue::MessageBufferType* messageBuffer) override
	{
		if (messageBuffer->size() < 2)
		{
			return;
		}

		MessageBase* message = messageBuffer->front().get();
		if (message->getType() == MessageErrorCountUpdate::getStaticType())
		{
			for (auto it = messageBuffer->begin() + 1; it != messageBuffer->end(); it++)
			{
				if ((*it)->getType() == MessageErrorCountUpdate::getStaticType())
				{
					messageBuffer->pop_front();
					return;
				}
			}
		}
	}
};

#endif // MESSAGE_FILTER_ERROR_COUNT_UPDATE_H
