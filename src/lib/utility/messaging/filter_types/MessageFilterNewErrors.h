#ifndef MESSAGE_FILTER_NEW_ERRORS_H
#define MESSAGE_FILTER_NEW_ERRORS_H

#include "utility/messaging/MessageFilter.h"
#include "utility/messaging/type/MessageNewErrors.h"

class MessageFilterNewErrors
	: public MessageFilter
{
	void filter(MessageQueue::MessageBufferType* messageBuffer) override
	{
		if (messageBuffer->size() < 2)
		{
			return;
		}

		MessageBase* message = messageBuffer->front().get();
		if (message->getType() == MessageNewErrors::getStaticType())
		{
			for (auto it = messageBuffer->begin() + 1; it != messageBuffer->end(); it++)
			{
				if ((*it)->getType() == MessageNewErrors::getStaticType())
				{
					MessageNewErrors* frontErrorsMessage = dynamic_cast<MessageNewErrors*>(message);
					MessageNewErrors* backErrorsMessage = dynamic_cast<MessageNewErrors*>(it->get());

					backErrorsMessage->errors.insert(
						backErrorsMessage->errors.begin(),
						frontErrorsMessage->errors.begin(),
						frontErrorsMessage->errors.end()
					);

					messageBuffer->pop_front();
					return;
				}
			}
		}
	}
};

#endif // MESSAGE_FILTER_NEW_ERRORS_H
